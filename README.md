# Telegram ncurses console chat
"Telegram secure chat" is a simple ncurses based telegram client
called in short **telesecure**

## Program features
* 11 conurent chats allocated and key F2 - F12 for quick access
* At F1 is a command console for telegram client, and it supports
    * Send quick message to the user without console allocation
    * Create and delete new chat
    * List channels and users
    * Display messages from all users and chats non allocated on buffers
* All opened chats are displayed in the top status bar with initials
    * Current active chat is higlighted with text underline
    * When the user is online on the chat initial on the status bar is yellow
    * When the user is typing message on the chat initial on status bar is blue
    * When the new message arrives from the user * char is added on the status bar

## Shortcuts
* **F1-F12**: Switch between chat buffers
* **CTRL+ArrowRight**: (_FN+ArrowRight (OSX)_): Switch to next chat buffer
* **CTRL+ArrowLeft**: (_FN+ArrowLeft (OSX)_): Switch to previous chat buffer
* **CTRL+C**: Exit telegram
* **CTRL+P**: run command PS on the last buffer
* **CTRL+ArrowUp** (_FN+ArrowUP (OSX)_): Rewind history buffer up
* **CTRL+ArrowDown** (_FN+ArrowDown (OSX)_): Rewind history buffer up
* **CTRL+A**: Clear edit window


## Supported commands
* **msg** ***chat_id message***: Send message to the user
* **authpass** ***password**: Send authentication password when required
* **delchat** ***chat_id***: Delete chat with particular chat identifier
* **authphoneno** ***phone_number***: Send phone number for authentication
* **authcode** ***code***: Pass authentication code
* **userlist**: List all user in address book
* **chatlist**: List all chats
* **help**: List all commands
* **authname** ***name***: Send user authentication name
* **newchat** ***chat_id***: Open chat with particular id
* **chatlist**: List all opened chats
* **quit** , **exit**: Close application

## Howto build ##
### Prerequisties ###
* C++ compiler with C++17 support (clang,g++)
* Linux or OSX operating system
* CMake 
* readline library _(libreadline)_
* telegram client library _(tdlib)_
* boost libraries
* ncurses _(libncurses)_
### Building ###
```console
mkdir build 
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## TODO ##
* Support for last seen time on users
* Maybe unlimited number of buffers with intelligent status bar 
* Uploading and downloading media files
