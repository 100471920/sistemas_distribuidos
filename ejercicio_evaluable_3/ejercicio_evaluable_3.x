struct mensaje {
    int key;
    string val_1<256>;
    double vector<32>;
};

program INNIT_PROG {
    version INNIT_VERSION {
        int INNIT() = 1;
    } = 1;
} = 0x20000001;

program SET_VALUE_PROG {
    version SET_VALUE_VERSION {
        int SET_VALUE(struct mensaje request) = 1;
    } = 2;
} = 0x20000002;

program GET_VALUE_PROG {
    version GET_VALUE_VERSION {
        struct mensaje GET_VALUE(int key) = 1;
    } = 3;
} = 0x20000003;

program MODIFY_VALUE_PROG {
    version MODIFY_VALUE_VERSION {
        int MODIFY_VALUE(struct mensaje) = 1;
    } = 4;
} = 0x20000004;

program DELETE_KEY_PROG {
    version DELETE_KEY_VERSION {
        int DELETE_KEY(int key) = 1;
    } = 5;
} = 0x20000005;

program EXIST_PROG {
    version EXIST_VERSION {
        int EXIST(int key) = 1;
    } = 6;
} = 0x20000006;