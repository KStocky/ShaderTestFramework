set(INTERNET_CONNECTION_AVAILABLE true)

execute_process(
        COMMAND ping www.bing.com -n 2 -w 1000
        ERROR_QUIET
        RESULT_VARIABLE CONNECTION_STATUS
    )

if (CONNECTION_STATUS GREATER 0)

    set(INTERNET_CONNECTION_AVAILABLE false)

endif()