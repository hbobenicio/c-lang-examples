# http-server :: TODO

- [ ] gracefull shutdown with signal handling (remember to revisit how poll works)
    - https://wiki.sei.cmu.edu/confluence/display/c/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers
    - set `volatile sig_atomic_t` for flaging the main loop that it needs to stop? what about the timeout?
 
