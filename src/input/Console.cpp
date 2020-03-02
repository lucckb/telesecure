#include <input/Console.hpp>

#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <unordered_map>

#include <cstdlib>
#include <cstring>
#include <readline/readline.h>
#include <readline/history.h>

namespace CppReadline {
    namespace {

        Console* currentConsole         = nullptr;
        HISTORY_STATE* emptyHistory     = history_get_history_state();

    }  /* namespace  */

    struct Console::Impl {
        using RegisteredCommands = std::unordered_map<std::string,Console::CommandFunction>;

        ::std::string       greeting_;
        // These are hardcoded commands. They do not do anything and are catched manually in the executeCommand function.
        RegisteredCommands  commands_;
        HISTORY_STATE*      history_    = nullptr;
        bool input_avail = false;
        int character;
        std::function<void()> redisplay;
        std::function<void(int)> command_completted;

        Impl(::std::string const& greeting) : greeting_(greeting), commands_() {}
        ~Impl() {
            free(history_);
        }

        Impl(Impl const&) = delete;
        Impl(Impl&&) = delete;
        Impl& operator = (Impl const&) = delete;
        Impl& operator = (Impl&&) = delete;
    };

    // Here we set default commands, they do nothing since we quit with them
    // Quitting behaviour is hardcoded in readLine()
    Console::Console(std::string const& greeting)
        : pimpl_{ new Impl{ greeting } }
    {
        reserveConsole();
        // Extra LB init
         if (rl_bind_key('\t', rl_insert))
            throw std::logic_error("Invalid key passed to rl_bind_key()");
        // Let ncurses do all terminal and signal handling
        rl_catch_signals = 0;
        rl_catch_sigwinch = 0;
        rl_deprep_term_function = NULL;
        rl_prep_term_function = NULL;
        // Prevent readline from setting the LINES and COLUMNS environment
        // variables, which override dynamic size adjustments in ncurses. When
         // using the alternate readline interface (as we do here), LINES and
        // COLUMNS are not updated if the terminal is resized between two calls to
        // rl_callback_read_char() (which is almost always the case).
        rl_change_environment = 0;

        // Handle input by manually feeding characters to readline
        rl_getc_function = [](FILE*) -> int {
            currentConsole->pimpl_->input_avail = false;
            return currentConsole->pimpl_->character;
        };
        rl_input_available_hook = []() -> int {
            return currentConsole->pimpl_->input_avail;
        };
        rl_redisplay_function = []() {
            if(currentConsole->pimpl_->redisplay) 
                currentConsole->pimpl_->redisplay();
        };
        rl_callback_handler_install(
            pimpl_->greeting_.c_str(),
            [](char* buffer) {
                if(!buffer) return;
                if ( buffer[0] != '\0' ) add_history(buffer);
                std::string line(buffer);
                free(buffer);
                if(currentConsole->pimpl_->command_completted)
                    currentConsole->pimpl_->command_completted(
                        currentConsole->executeCommand(line)
                    );
            }
        );
        // Extra LB end
        // Init readline basics
        rl_attempted_completion_function = &Console::getCommandCompletions;

  
        // Quit and Exit simply terminate the console.
        pimpl_->commands_["quit"] = [this](const Arguments &) {
            return ReturnCode::Quit;
        };

        pimpl_->commands_["exit"] = [this](const Arguments &) {
            return ReturnCode::Quit;
        };
    }

    Console::~Console() = default;

    void Console::registerCommand(const std::string & s, CommandFunction f) {
        pimpl_->commands_[s] = f;
    }

    std::vector<std::string> Console::getRegisteredCommands() const {
        std::vector<std::string> allCommands;
        for ( auto & pair : pimpl_->commands_ ) allCommands.push_back(pair.first);

        return allCommands;
    }

    void Console::saveState() {
        free(pimpl_->history_);
        pimpl_->history_ = history_get_history_state();
    }

    void Console::reserveConsole() {
        if ( currentConsole == this ) return;

        // Save state of other Console
        if ( currentConsole )
            currentConsole->saveState();

        // Else we swap state
        if ( ! pimpl_->history_ )
            history_set_history_state(emptyHistory);
        else
            history_set_history_state(pimpl_->history_);

        // Tell others we are using the console
        currentConsole = this;
    }

    void Console::setGreeting(const std::string & greeting) {
        pimpl_->greeting_ = greeting;
    }

    std::string Console::getGreeting() const {
        return pimpl_->greeting_;
    }

    int Console::executeCommand(const std::string & command) {
        // Convert input to vector
        std::vector<std::string> inputs;
        {
            std::istringstream iss(command);
            std::copy(std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    std::back_inserter(inputs));
        }

        if ( inputs.size() == 0 ) return ReturnCode::Ok;

        Impl::RegisteredCommands::iterator it;
        if ( ( it = pimpl_->commands_.find(inputs[0]) ) != end(pimpl_->commands_) ) {
            return static_cast<int>((it->second)(inputs));
        }

        return ReturnCode::Error;
    }

    int Console::executeFile(const std::string & filename) {
        std::ifstream input(filename);
        if ( ! input ) {
            return ReturnCode::Error;
        }
        std::string command;
        int counter = 0, result;

        while ( std::getline(input, command)  ) {
            if ( command[0] == '#' ) continue; // Ignore comments
            // Report what the Console is executing. 
            if ( (result = executeCommand(command)) ) return result; 
        }

        // If we arrived successfully at the end, all is ok
        return ReturnCode::Ok;
    }

    int Console::readLine() {
        reserveConsole();

        char * buffer = readline(pimpl_->greeting_.c_str());
        if ( !buffer ) {
            std::cout << '\n'; // EOF doesn't put last endline so we put that so that it looks uniform.
            return ReturnCode::Quit;
        }

        // TODO: Maybe add commands to history only if succeeded?
        if ( buffer[0] != '\0' )
            add_history(buffer);

        std::string line(buffer);
        free(buffer);

        return executeCommand(line);
    }

    char ** Console::getCommandCompletions(const char * text, int start, int) {
        char ** completionList = nullptr;

        if ( start == 0 )
            completionList = rl_completion_matches(text, &Console::commandIterator);

        return completionList;
    }

    char * Console::commandIterator(const char * text, int state) {
        static Impl::RegisteredCommands::iterator it;
        if (!currentConsole)
            return nullptr;
        auto& commands = currentConsole->pimpl_->commands_;

        if ( state == 0 ) it = begin(commands);

        while ( it != end(commands) ) {
            auto & command = it->first;
            ++it;
            if ( command.find(text) != std::string::npos ) {
                return strdup(command.c_str());
            }
        }
        return nullptr;
    }
    void Console::registerRedisplayCommand(std::function<void()> redisp )
    {
        currentConsole->pimpl_->redisplay = redisp;
    }

    void Console::registerCommandCompleted( std::function<void(int)> cmdcpl )
    {
        currentConsole->pimpl_->command_completted = cmdcpl;
    }

    void Console::forwardToReadline(int chr)
    {
        currentConsole->pimpl_->character = chr;
        currentConsole->pimpl_->input_avail = true;
        rl_callback_read_char();
    }
}
