    struct usuario_operacion {
        string usuario<256>;
        string operacion<20>;
        string fecha<20>;
        string file_name<256>;
    };

    program OPERACIONES_PROG {
        version OPERACIONES_VERS {
            void imprimir_operacion_usuario(usuario_operacion) = 1;
        } = 1;
    } = 0x31230000;
