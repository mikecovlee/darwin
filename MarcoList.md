###Marco List:
**DARWIN_FORCE_BUILTIN**:Darwin will use built-in modules instead of dynamically loaded modules. But you still need to load a module at the beginning of the program (as long as you use a name that has no effect on your program) to start the Darwin function.  
**DARWIN_FORCE_UNIX**:Darwin will force use unix headers.  
**DARWIN_FORCE_WIN32**:Darwin will force use win32 headers.  
**DARWIN_STRICT_CHECK**:Darwin will terminate the program if there have any warning.  
**DARWIN_IGNORE_WARNING**:Darwin will ignore every warning and stop writing warnings to log file.  
**DARWIN_DISABLE_LOG**:Directly disable the log.