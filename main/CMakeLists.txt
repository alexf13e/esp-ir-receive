idf_component_register(SRCS "remote_receiver.cpp"
    INCLUDE_DIRS "."
    REQUIRES slint)

slint_target_sources(${COMPONENT_LIB} ui/app-window.slint)
