function(setup_polyglot pem_file executable_name)
    set(POLYGLOT_PUBLIC_PEM
        ${pem_file}
        CACHE
        INTERNAL
        "pem file"
        FORCE
    )
    set(POLYGLOT_EXECUTABLE
        ${executable_name}
        CACHE
        INTERNAL
        "executable name"
        FORCE
    )
endfunction()
