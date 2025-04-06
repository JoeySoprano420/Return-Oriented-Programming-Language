# Add hardened security flags
target_compile_options(roplang PRIVATE
    -Wall -O3
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -Wno-deprecated-declarations
    -fPIE -fPIC
)

target_link_options(roplang PRIVATE
    -Wl,-z,relro
    -Wl,-z,now
    -Wl,-z,noexecstack
    -pie
)
