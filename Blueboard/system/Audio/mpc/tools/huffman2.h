mpc_lut_data mpc_HuffHdr = {mpc_table_HuffHdr, {{2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {2, -1}, {4, -2}, {4, -2}, {4, -2}, {4, -2}, {5, -3}, {5, -3}, {6, 2}, {0, 2}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}} };
mpc_lut_data mpc_HuffDSCF = {mpc_table_HuffDSCF, {{3, -2}, {3, -2}, {3, -2}, {3, -2}, {3, -2}, {3, -2}, {3, -2}, {3, -2}, {5, -6}, {5, -6}, {5, 6}, {5, 6}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, -1}, {3, -1}, {3, -1}, {3, -1}, {3, -1}, {3, -1}, {3, -1}, {3, -1}, {6, -7}, {6, 7}, {5, -5}, {5, -5}, {4, 0}, {4, 0}, {4, 0}, {4, 0}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, 1}, {4, 8}, {4, 8}, {4, 8}, {4, 8}, {4, -3}, {4, -3}, {4, -3}, {4, -3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {5, -4}, {5, -4}, {5, 5}, {5, 5}} };
mpc_lut_data mpc_HuffQ [7] [2] = {
{{mpc_table_HuffQ1[0], {{0, -40}, {6, 0}, {0, -44}, {6, 20}, {0, -48}, {6, 6}, {0, -64}, {4, 14}, {0, -80}, {4, 12}, {0, -96}, {4, 4}, {0, -112}, {4, 22}, {0, -116}, {6, 8}, {0, -120}, {6, 18}, {0, -124}, {6, 24}, {0, -128}, {6, 2}, {0, 112}, {4, 16}, {0, 96}, {4, 10}, {0, 88}, {5, 17}, {0, 80}, {5, 9}, {0, 72}, {5, 1}, {0, 64}, {5, 25}, {0, 56}, {5, 5}, {0, 48}, {5, 21}, {0, 40}, {5, 3}, {0, 32}, {5, 11}, {0, 24}, {5, 15}, {0, 16}, {5, 23}, {0, 8}, {5, 19}, {0, 0}, {5, 7}, {0, -128}, {1, 13}, {0, 126}, {7, 15}, {0, 124}, {7, 1}, {0, 122}, {7, 11}, {0, 120}, {7, 7}, {0, 118}, {7, 17}, {0, 116}, {7, 25}} }, {mpc_table_HuffQ1[1], {{0, 113}, {9, 18}, {128, 112}, {9, 2}, {0, 112}, {9, 24}, {0, 110}, {7, 3}, {0, 108}, {7, 23}, {0, 106}, {7, 21}, {0, 104}, {7, 5}, {0, 103}, {8, 0}, {0, 102}, {8, 26}, {0, 101}, {8, 6}, {0, 100}, {8, 20}, {0, 96}, {6, 9}, {0, 80}, {4, 14}, {0, 64}, {4, 12}, {0, 48}, {4, 4}, {0, 32}, {4, 22}, {0, 16}, {4, 16}, {0, 0}, {4, 10}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -16}, {4, 13}, {0, -32}, {4, 17}, {0, -48}, {4, 7}, {0, -64}, {4, 11}, {0, -68}, {6, 1}, {0, -72}, {6, 23}, {0, -74}, {7, 4}, {0, -76}, {7, 20}, {0, -78}, {7, 0}, {0, -80}, {7, 24}, {0, -88}, {5, 22}, {0, -96}, {5, 10}} }},
{{mpc_table_HuffQ2[0], {{0, 112}, {5, 14}, {0, 96}, {4, 6}, {0, 80}, {4, 18}, {0, 64}, {4, 8}, {0, 48}, {4, 16}, {0, 40}, {5, 9}, {0, 32}, {5, 5}, {0, 24}, {5, 15}, {0, 16}, {5, 21}, {0, 8}, {5, 19}, {0, 0}, {5, 3}, {0, -8}, {5, 18}, {0, -16}, {5, 6}, {0, -24}, {5, 8}, {0, -25}, {8, 3}, {192, -26}, {10, 24}, {128, -26}, {10, 4}, {64, -26}, {10, 0}, {0, -26}, {10, 20}, {0, -28}, {7, 23}, {0, -30}, {7, 1}, {0, -32}, {7, 19}, {0, -40}, {5, 16}, {0, -42}, {7, 15}, {0, -44}, {7, 21}, {0, -46}, {7, 9}, {0, -48}, {7, 5}, {0, -52}, {6, 2}, {0, -56}, {6, 10}, {0, -60}, {6, 14}, {0, -64}, {6, 22}, {0, -128}, {2, 12}} }, {mpc_table_HuffQ2[1], {{0, 32}, {3, 11}, {0, 0}, {3, 7}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -32}, {3, 1}, {0, -48}, {4, 3}, {0, -64}, {4, -3}, {0, -96}, {3, 2}, {0, -128}, {3, -2}, {0, 64}, {2, 0}, {0, 0}, {2, -1}, {0, -64}, {2, 0}, {0, -128}, {2, -1}, {0, 64}, {2, 1}, {0, 48}, {4, -2}, {0, 40}, {5, 3}, {0, 32}, {5, -3}, {0, 0}, {3, 2}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -32}, {3, 0}, {0, -64}, {3, -1}, {0, -96}, {3, 1}, {0, -128}, {3, -2}, {0, 96}, {3, 2}, {0, 80}, {4, -4}, {0, 64}, {4, 4}, {0, 32}, {3, 3}} }},
{{mpc_table_HuffQ3[0], {{0, -48}, {4, 2}, {0, -64}, {4, -3}, {0, -128}, {2, 0}, {0, 96}, {3, -2}, {0, 80}, {4, 3}, {0, 72}, {5, -4}, {0, 64}, {5, 4}, {0, 0}, {2, -1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -16}, {4, 2}, {0, -24}, {5, 5}, {0, -28}, {6, -7}, {0, -32}, {6, 7}, {0, -48}, {4, -3}, {0, -64}, {4, 3}, {0, -72}, {5, -6}, {0, -80}, {5, 6}, {0, -96}, {4, -4}, {0, -112}, {4, 4}, {0, -128}, {4, -5}, {0, 96}, {3, 0}, {0, 64}, {3, -1}, {0, 32}, {3, 1}, {0, 0}, {3, -2}, {0, -16}, {4, 3}, {0, -24}, {5, 4}, {0, -26}, {7, 6}} }, {mpc_table_HuffQ3[1], {{0, -32}, {6, -6}, {0, -64}, {3, 0}, {0, -96}, {3, -1}, {0, -128}, {3, 1}, {0, 96}, {3, -2}, {0, 64}, {3, 2}, {0, 56}, {5, -5}, {0, 48}, {5, 5}, {0, 32}, {4, -4}, {0, 0}, {3, -3}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -8}, {5, 3}, {0, -16}, {5, -4}, {0, -20}, {6, -11}, {0, -24}, {6, 12}, {0, -32}, {5, 4}, {0, -40}, {5, 6}, {0, -48}, {5, -5}, {0, -56}, {5, 5}, {0, -64}, {5, 7}, {0, -72}, {5, -7}, {0, -76}, {6, -12}, {0, -80}, {6, -13}, {0, -88}, {5, -6}, {0, -96}, {5, 8}, {0, -104}, {5, -8}, {0, -112}, {5, 9}, {0, -120}, {5, -9}, {0, -124}, {6, 13}, {0, -126}, {7, -15}, {0, -128}, {7, 15}} }},
{{mpc_table_HuffQ4[0], {{0, 96}, {5, 10}, {0, 80}, {4, -1}, {0, 64}, {4, 2}, {0, 48}, {4, 1}, {0, 32}, {4, -2}, {0, 28}, {6, 14}, {0, 24}, {6, -14}, {0, 16}, {5, 11}, {0, 0}, {4, -3}, {0, -8}, {5, -6}, {0, -16}, {5, 6}, {0, -32}, {4, 1}, {0, -48}, {4, -1}, {0, -50}, {7, 10}, {0, -52}, {7, -10}, {0, -53}, {8, -11}, {128, -54}, {9, -12}, {96, -54}, {11, 13}, {88, -54}, {13, 15}, {80, -54}, {13, -14}, {72, -54}, {13, 14}, {64, -54}, {13, -15}, {0, -54}, {10, -13}, {0, -55}, {8, 11}, {0, -56}, {8, 12}, {0, -60}, {6, -9}, {0, -64}, {6, 9}, {0, -80}, {4, -2}, {0, -96}, {4, 2}, {0, -112}, {4, 3}, {0, -128}, {4, -3}, {0, 120}, {5, -7}} }, {mpc_table_HuffQ4[1], {{0, 80}, {4, 4}, {0, 72}, {5, -8}, {0, 64}, {5, 8}, {0, 48}, {4, 5}, {0, 32}, {4, -5}, {0, 0}, {3, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -4}, {6, 7}, {0, -8}, {6, 8}, {0, -12}, {6, 9}, {0, -16}, {6, -8}, {0, -20}, {6, 11}, {0, -22}, {7, 21}, {0, -23}, {8, -28}, {0, -24}, {8, 28}, {0, -28}, {6, -9}, {0, -30}, {7, -22}, {0, -32}, {7, -21}, {0, -36}, {6, -10}, {0, -40}, {6, -11}, {0, -44}, {6, 10}, {0, -48}, {6, 12}, {0, -52}, {6, -13}, {0, -54}, {7, 22}, {0, -56}, {7, 23}, {0, -60}, {6, -12}, {0, -64}, {6, 13}, {0, -68}, {6, 14}, {0, -72}, {6, -14}, {0, -74}, {7, -23}, {0, -75}, {8, -29}} }},
{{mpc_table_HuffQ5[0], {{0, -84}, {6, 15}, {0, -88}, {6, 16}, {0, -92}, {6, -16}, {0, -94}, {7, -24}, {0, -96}, {7, 24}, {0, -100}, {6, 17}, {0, -102}, {7, -25}, {0, -103}, {8, -30}, {0, -104}, {8, 30}, {0, -108}, {6, -17}, {0, -112}, {6, 18}, {0, -116}, {6, -18}, {0, -118}, {7, 25}, {0, -120}, {7, 26}, {0, -124}, {6, 19}, {0, -126}, {7, -26}, {0, -128}, {7, -27}, {0, 120}, {5, 2}, {0, 116}, {6, -19}, {0, 112}, {6, 20}, {0, 104}, {5, -1}, {0, 103}, {8, -31}, {0, 102}, {8, 31}, {0, 100}, {7, 27}, {0, 96}, {6, -20}, {0, 88}, {5, 1}, {0, 80}, {5, -5}, {0, 72}, {5, -3}, {0, 64}, {5, 3}, {0, 56}, {5, 0}, {0, 48}, {5, -2}, {0, 40}, {5, -4}} }, {mpc_table_HuffQ5[1], {{0, 16}, {5, -6}, {0, 8}, {5, 6}, {0, 0}, {5, -7}, {0, -8}, {5, -1}, {0, -16}, {5, 2}, {0, -24}, {5, -2}, {0, -32}, {5, 3}, {0, -33}, {8, -20}, {192, -34}, {10, 24}, {188, -34}, {14, 28}, {184, -34}, {14, -28}, {180, -34}, {14, -30}, {176, -34}, {14, 30}, {160, -34}, {12, -27}, {156, -34}, {14, 29}, {152, -34}, {14, -29}, {148, -34}, {14, 31}, {144, -34}, {14, -31}, {128, -34}, {12, 27}, {0, -34}, {9, -22}, {0, -36}, {7, -17}, {0, -40}, {6, -11}, {0, -48}, {5, -3}, {0, -56}, {5, 4}, {0, -64}, {5, -4}, {0, -66}, {7, 17}, {0, -67}, {8, 20}, {128, -68}, {9, 22}, {64, -68}, {10, -25}, {0, -68}, {10, -26}, {0, -72}, {6, 12}, {0, -80}, {5, 5}} }},
{{mpc_table_HuffQ6[0], {{0, -104}, {5, -6}, {0, -108}, {6, -12}, {0, -110}, {7, -18}, {0, -112}, {7, 18}, {0, -116}, {6, 13}, {0, -120}, {6, -13}, {0, -128}, {5, -7}, {0, 124}, {6, 14}, {0, 123}, {8, 21}, {0, 122}, {8, -21}, {0, 120}, {7, -19}, {0, 112}, {5, 7}, {0, 104}, {5, 8}, {0, 100}, {6, -14}, {0, 96}, {6, -15}, {0, 88}, {5, -8}, {0, 84}, {6, 15}, {0, 82}, {7, 19}, {192, 81}, {10, 25}, {128, 81}, {10, 26}, {0, 81}, {9, -23}, {128, 80}, {9, 23}, {0, 80}, {9, -24}, {0, 72}, {5, -9}, {0, 64}, {5, 9}, {0, 60}, {6, 16}, {0, 56}, {6, -16}, {0, 48}, {5, 10}, {0, 32}, {4, 0}, {0, 24}, {5, -10}, {0, 16}, {5, 11}, {0, 0}, {4, 1}} }, {mpc_table_HuffQ6[1], {{0, 0}, {3, 3}, {2, 3}, {1, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -128}, {2, 3}, {0, 64}, {3, 5}, {0, 24}, {5, 11}, {0, 4}, {6, 14}, {0, 0}, {7, 15}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {15, 10}, {14, 11}, {13, 9}, {7, 6}, {5, 12}, {8, 3}, {2, 0}, {4, 1}, {0, -96}, {3, 7}, {0, 64}, {4, 12}, {0, 40}, {5, 16}, {0, 24}, {6, 21}, {0, 14}, {7, 27}, {0, 7}, {8, 34}, {128, 3}, {9, 41}, {64, 1}, {10, 48}, {128, 0}, {11, 53}, {48, 0}, {12, 57}, {24, 0}, {13, 60}, {0, 0}, {14, 63}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}} }},
{{mpc_table_HuffQ7[0], {{26, 37}, {28, 25}, {39, 38}, {24, 23}, {40, 22}, {21, 20}, {19, 43}, {42, 41}, {18, 17}, {16, 15}, {46, 45}, {44, 14}, {13, 12}, {11, 49}, {48, 47}, {31, 10}, {9, 8}, {7, 6}, {52, 51}, {50, 5}, {4, 3}, {54, 53}, {2, 1}, {0, 57}, {56, 55}, {63, 62}, {61, 60}, {59, 58}, {0, 96}, {3, 7}, {0, 48}, {4, 10}, {0, 24}, {5, 13}, {0, 16}, {6, 16}, {0, 10}, {7, 20}, {0, 6}, {8, 25}, {128, 3}, {9, 31}, {192, 1}, {10, 38}, {224, 0}, {11, 45}, {80, 0}, {12, 52}, {32, 0}, {13, 57}, {12, 0}, {14, 61}, {0, 0}, {15, 64}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {33, 32}, {31, 30}, {29, 34}, {28, 27}} }, {mpc_table_HuffQ7[1], {{40, 39}, {22, 21}, {42, 41}, {20, 19}, {18, 45}, {44, 43}, {17, 16}, {15, 14}, {48, 47}, {46, 13}, {12, 11}, {10, 64}, {52, 51}, {50, 49}, {9, 8}, {7, 6}, {55, 54}, {53, 5}, {4, 3}, {58, 57}, {56, 2}, {1, 63}, {62, 61}, {60, 59}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -128}, {1, 1}, {0, 64}, {2, 2}, {0, 32}, {3, 3}, {0, 16}, {5, 6}, {0, 8}, {6, 8}, {0, 6}, {7, 10}, {0, 3}, {8, 13}, {0, 2}, {9, 16}, {64, 1}, {10, 20}, {192, 0}, {11, 25}, {16, 0}, {12, 31}, {0, 0}, {13, 32}} }}
};
mpc_can_data mpc_can_SCFI[2] = { {mpc_huff_SCFI_1, mpc_sym_SCFI_1, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -128}, {2, 3}, {0, 64}, {3, 5}, {0, 24}, {5, 11}, {0, 4}, {6, 14}, {0, 0}, {7, 15}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {15, 10}, {14, 11}, {13, 9}, {7, 6}, {5, 12}, {8, 3}, {2, 0}, {4, 1}, {0, -96}, {3, 7}, {0, 64}, {4, 12}, {0, 40}, {5, 16}, {0, 24}, {6, 21}, {0, 14}, {7, 27}, {0, 7}, {8, 34}, {128, 3}, {9, 41}, {64, 1}, {10, 48}, {128, 0}, {11, 53}, {48, 0}, {12, 57}, {24, 0}, {13, 60}, {0, 0}, {14, 63}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {35, 34}, {33, 36}, {32, 30}, {29, 27}} }, {mpc_huff_SCFI_2, mpc_sym_SCFI_2, {{18, 17}, {16, 15}, {46, 45}, {44, 14}, {13, 12}, {11, 49}, {48, 47}, {31, 10}, {9, 8}, {7, 6}, {52, 51}, {50, 5}, {4, 3}, {54, 53}, {2, 1}, {0, 57}, {56, 55}, {63, 62}, {61, 60}, {59, 58}, {0, 96}, {3, 7}, {0, 48}, {4, 10}, {0, 24}, {5, 13}, {0, 16}, {6, 16}, {0, 10}, {7, 20}, {0, 6}, {8, 25}, {128, 3}, {9, 31}, {192, 1}, {10, 38}, {224, 0}, {11, 45}, {80, 0}, {12, 52}, {32, 0}, {13, 57}, {12, 0}, {14, 61}, {0, 0}, {15, 64}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {33, 32}, {31, 30}, {29, 34}, {28, 27}, {36, 35}, {26, 37}, {25, 38}, {24, 23}, {40, 39}, {22, 21}, {42, 41}, {20, 19}} } };
mpc_can_data mpc_can_DSCF[2] = { {mpc_huff_DSCF_1, mpc_sym_DSCF_1, {{0, 14}, {7, 27}, {0, 7}, {8, 34}, {128, 3}, {9, 41}, {64, 1}, {10, 48}, {128, 0}, {11, 53}, {48, 0}, {12, 57}, {24, 0}, {13, 60}, {0, 0}, {14, 63}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {35, 34}, {33, 36}, {32, 30}, {29, 27}, {26, 37}, {28, 25}, {39, 38}, {24, 23}, {40, 22}, {21, 20}, {19, 43}, {42, 41}, {18, 17}, {16, 15}, {46, 45}, {44, 14}, {13, 12}, {11, 49}, {48, 47}, {31, 10}, {9, 8}, {7, 6}, {52, 51}, {50, 5}, {4, 3}, {54, 53}, {2, 1}, {0, 57}, {56, 55}, {63, 62}, {61, 60}, {59, 58}, {0, 96}, {3, 7}, {0, 48}, {4, 10}, {0, 24}, {5, 13}, {0, 16}, {6, 16}} }, {mpc_huff_DSCF_2, mpc_sym_DSCF_2, {{224, 0}, {11, 45}, {80, 0}, {12, 52}, {32, 0}, {13, 57}, {12, 0}, {14, 61}, {0, 0}, {15, 64}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {33, 32}, {31, 30}, {29, 34}, {28, 27}, {36, 35}, {26, 37}, {25, 38}, {24, 23}, {40, 39}, {22, 21}, {42, 41}, {20, 19}, {18, 45}, {44, 43}, {17, 16}, {15, 14}, {48, 47}, {46, 13}, {12, 11}, {10, 64}, {52, 51}, {50, 49}, {9, 8}, {7, 6}, {55, 54}, {53, 5}, {4, 3}, {58, 57}, {56, 2}, {1, 63}, {62, 61}, {60, 59}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}} } };
mpc_can_data mpc_can_Bands = {mpc_huff_Bands, mpc_sym_Bands, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}} };
mpc_can_data mpc_can_Res[2] = { {mpc_huff_Res_1, mpc_sym_Res_1, {{0, 8}, {5, 5}, {0, 4}, {6, 6}, {0, 2}, {7, 7}, {0, 1}, {8, 8}, {128, 0}, {9, 9}, {64, 0}, {10, 10}, {32, 0}, {11, 11}, {16, 0}, {12, 12}, {8, 0}, {13, 13}, {4, 0}, {14, 14}, {2, 0}, {15, 15}, {0, 0}, {16, 16}, {0, 1}, {16, 2}, {3, 4}, {5, 15}, {6, 7}, {8, 9}, {10, 11}, {12, 14}, {13, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 64}, {2, 3}, {0, 32}, {3, 4}, {0, 16}, {4, 5}, {0, 8}, {5, 6}, {0, 4}, {6, 7}, {0, 2}, {7, 8}, {0, 1}, {8, 9}, {128, 0}, {9, 10}, {64, 0}, {10, 11}, {32, 0}, {11, 12}, {16, 0}, {12, 13}, {0, 0}, {14, 16}} }, {mpc_huff_Res_2, mpc_sym_Res_2, {{8, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 96}, {3, 7}, {0, 16}, {4, 10}, {0, 8}, {5, 11}, {0, 4}, {6, 12}, {0, 2}, {7, 13}, {0, 1}, {8, 14}, {128, 0}, {9, 15}, {64, 0}, {10, 16}, {32, 0}, {11, 17}, {0, 0}, {12, 18}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {7, 6}, {5, 4}, {3, 10}, {9, 8}, {2, 1}, {11, 0}, {12, 13}, {14, 15}, {16, 18}, {17, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}} } };
mpc_can_data mpc_can_Q1 = {mpc_huff_Q1, mpc_sym_Q1, {{0, 4}, {6, 0}, {5, 11}, {5, 11}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 2}, {4, 2}, {4, 2}, {4, 2}, {4, 8}, {4, 8}, {4, 8}, {4, 8}, {4, 9}, {4, 9}, {4, 9}, {4, 9}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 4}, {3, 4}, {3, 4}, {3, 4}, {3, 4}, {3, 4}, {3, 4}, {3, 4}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 7}, {3, 7}, {3, 7}, {3, 7}, {3, 7}, {3, 7}, {3, 7}, {3, 7}} };
mpc_can_data mpc_can_Q9up = {mpc_huff_Q9up, mpc_sym_Q9up, {{0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {6, 127}, {6, -128}} };
mpc_can_data mpc_can_Q [6][2] = {
{{mpc_huff_Q2_1, mpc_sym_Q2_1, {{0, 18}, {8, 74}, {0, 6}, {9, 92}, {192, 3}, {10, 104}, {96, 0}, {11, 119}, {32, 0}, {12, 122}, {0, 0}, {13, 124}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {62, 87}, {67, 63}, {61, 57}, {37, 93}, {92, 88}, {86, 83}, {82, 81}, {68, 66}, {58, 56}, {42, 41}, {38, 36}, {32, 31}, {112, 91}, {72, 64}, {60, 52}, {43, 33}, {12, 117}, {113, 111}, {107, 97}, {89, 85}, {77, 73}, {71, 69}, {65, 59}, {55, 53}, {51, 47}, {39, 35}, {27, 17}, {13, 11}, {7, 118}, {116, 108}, {106, 98}, {96, 94}, {90, 84}, {80, 78}, {76, 48}, {46, 44}, {40, 34}, {30, 28}, {26, 18}, {16, 8}} }, {mpc_huff_Q2_2, mpc_sym_Q2_2, {{105, 103}, {101, 99}, {95, 79}, {75, 49}, {45, 29}, {25, 23}, {21, 19}, {15, 14}, {10, 9}, {5, 3}, {1, 124}, {104, 20}, {0, 120}, {100, 24}, {4, 0}, {0, 0}, {0, -16}, {4, 15}, {0, 112}, {5, 30}, {0, 72}, {6, 44}, {0, 60}, {7, 62}, {0, 12}, {8, 92}, {128, 7}, {9, 104}, {192, 0}, {10, 119}, {64, 0}, {11, 122}, {0, 0}, {12, 124}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {62, 92}, {87, 86}, {82, 68}, {67, 66}, {63, 61}, {58, 57}, {56, 42}, {38, 37}, {32, 93}, {91, 88}, {83, 81}, {43, 41}, {36, 33}, {31, 112}, {72, 64}, {60, 52}} }},
{{mpc_huff_Q3, mpc_sym_Q3, {{84, 80}, {78, 77}, {76, 73}, {71, 69}, {65, 59}, {55, 53}, {51, 48}, {47, 46}, {44, 40}, {39, 35}, {34, 30}, {28, 27}, {26, 18}, {17, 16}, {13, 11}, {8, 7}, {6, 122}, {110, 74}, {70, 54}, {50, 22}, {14, 2}, {123, 121}, {119, 115}, {114, 109}, {105, 103}, {102, 101}, {99, 95}, {79, 75}, {49, 45}, {29, 25}, {23, 21}, {19, 15}, {10, 9}, {5, 3}, {1, 124}, {104, 20}, {0, 120}, {100, 24}, {4, 0}, {0, 0}, {0, -32}, {3, 7}, {0, -128}, {4, 14}, {0, 80}, {5, 22}, {0, 36}, {6, 32}, {0, 10}, {7, 41}, {0, 2}, {8, 46}, {0, 0}, {9, 48}, {0, 0}, {0, 0}, {0, 17}, {16, 1}, {15, -16}, {255, 32}, {31, 2}, {14, -15}, {224, 34}, {33, 47}} }, {mpc_huff_Q4, mpc_sym_Q4, {{253, -30}, {209, -48}, {223, 50}, {62, 35}, {45, -29}, {237, -46}, {222, 51}, {61, -45}, {221, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -16}, {4, 15}, {0, -112}, {5, 30}, {0, 52}, {6, 48}, {0, 24}, {7, 61}, {0, 5}, {8, 73}, {0, 1}, {9, 78}, {0, 0}, {10, 80}, {0, 0}, {0, 90}, {0, 32}, {17, 16}, {31, 2}, {1, 15}, {14, -15}, {240, -1}, {224, 49}, {48, 34}, {33, 47}, {46, 19}, {18, 30}, {29, 3}, {13, -13}, {242, -2}, {253, -30}, {225, -17}, {238, -47}, {208, -33}, {64, 50}, {63, 62}, {35, 45}, {4, 12}, {227, -19}, {210, -34}, {192, -49}, {66, 65}, {79, 78}, {51, 61}, {36, 44}, {20, 28}, {244, -4}, {228, -20}} }},
{{mpc_huff_Q5_1, mpc_sym_Q5_1, {{204, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -64}, {2, 3}, {0, 64}, {3, 6}, {0, 32}, {4, 8}, {0, 16}, {5, 10}, {0, 8}, {6, 12}, {0, 0}, {7, 14}, {0, 2}, {1, -1}, {254, 3}, {253, 4}, {252, 5}, {251, 7}, {6, -6}, {249, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 96}, {3, 7}, {0, 32}, {4, 10}, {0, 16}, {5, 12}, {0, 0}, {6, 14}, {2, 1}, {0, -1}, {254, 4}, {3, -3}, {252, 5}, {251, 7}, {6, -6}, {249, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, -64}, {2, 3}, {0, -128}, {3, 6}, {0, 64}, {4, 10}, {0, 40}, {5, 14}} }, {mpc_huff_Q5_2, mpc_sym_Q5_2, {{0, 1}, {255, 3}, {2, -2}, {253, 4}, {252, -5}, {8, 7}, {6, 5}, {250, -7}, {248, 9}, {247, 11}, {10, -10}, {245, 15}, {14, 13}, {12, -12}, {243, -14}, {241, 0}, {0, 80}, {4, 15}, {0, 32}, {5, 20}, {0, 16}, {6, 24}, {0, 4}, {7, 28}, {0, 0}, {8, 30}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {5, 4}, {3, 2}, {1, 0}, {255, -2}, {253, -4}, {251, 8}, {7, 6}, {250, -7}, {248, 10}, {9, -9}, {246, 13}, {12, 11}, {245, -12}, {243, 15}, {14, -14}, {241, 0}, {0, -64}, {2, 3}, {0, -128}, {3, 6}, {0, 96}, {4, 10}, {0, 64}, {5, 16}, {0, 40}, {6, 24}, {0, 20}, {7, 34}, {0, 10}, {8, 44}, {0, 4}, {9, 54}} }},
{{mpc_huff_Q6_1, mpc_sym_Q6_1, {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 1}, {255, 2}, {254, 4}, {3, -3}, {252, 7}, {6, 5}, {251, -6}, {249, 13}, {11, 10}, {9, 8}, {248, -9}, {246, -11}, {244, 17}, {16, 15}, {14, 12}, {243, -14}, {241, -16}, {239, 28}, {27, 21}, {20, 19}, {18, -18}, {237, -20}, {235, -27}, {228, 31}, {30, 29}, {26, 25}, {24, 23}, {22, -22}, {233, -24}, {231, -26}, {227, -30}, {225, 0}, {0, 96}, {5, 31}, {0, 36}, {6, 43}, {0, 16}, {7, 52}, {0, 2}, {8, 60}, {0, 0}, {9, 62}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {10, 9}, {8, 7}, {6, 5}, {4, 3}, {2, 1}, {0, -1}, {254, -3}, {252, -5}} }, {mpc_huff_Q6_2, mpc_sym_Q6_2, {{241, -16}, {239, 22}, {21, 20}, {19, 18}, {238, -19}, {236, -21}, {234, 29}, {28, 27}, {26, 25}, {24, 23}, {233, -24}, {231, -26}, {229, -28}, {227, 31}, {30, -30}, {225, 0}, {0, -64}, {2, 3}, {0, -128}, {3, 6}, {0, 112}, {4, 10}, {0, 88}, {5, 17}, {0, 56}, {6, 28}, {0, 40}, {7, 42}, {0, 25}, {8, 62}, {0, 13}, {9, 87}, {128, 2}, {10, 113}, {96, 0}, {11, 123}, {0, 0}, {12, 126}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 1}, {255, -2}, {3, 2}, {253, 7}, {6, 5}, {4, -4}, {251, -6}, {249, 11}, {10, 9}, {8, -8}, {247, -10}, {245, 19}, {18, 17}, {16, 15}, {14, 13}, {12, -12}} }},
{{mpc_huff_Q7_1, mpc_sym_Q7_1, {{21, 20}, {238, -20}, {235, -22}, {233, -24}, {231, -26}, {229, -33}, {202, -56}, {63, 62}, {61, 60}, {59, 58}, {57, 54}, {53, 43}, {40, 39}, {38, 37}, {36, 35}, {34, 33}, {32, 30}, {29, -28}, {227, -30}, {225, -32}, {222, -35}, {220, -37}, {218, -39}, {216, -41}, {213, -53}, {201, -57}, {198, -59}, {196, -61}, {49, 47}, {46, 45}, {44, 42}, {41, -42}, {212, -45}, {210, -47}, {208, -49}, {206, -62}, {193, 52}, {51, 50}, {48, -51}, {204, 0}, {0, -104}, {6, 63}, {0, 42}, {7, 101}, {0, 4}, {8, 122}, {0, 0}, {9, 126}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {13, 11}, {10, 9}, {8, 7}, {6, 5}, {4, 3}, {2, 1}, {0, -1}, {254, -3}} }, {mpc_huff_Q7_2, mpc_sym_Q7_2, {{33, 32}, {31, 30}, {29, 28}, {27, 26}, {25, 24}, {23, 22}, {21, 20}, {19, 18}, {17, 16}, {15, 14}, {12, -14}, {241, -16}, {239, -18}, {237, -20}, {235, -22}, {233, -24}, {231, -26}, {229, -28}, {227, -30}, {225, -32}, {223, -34}, {221, -36}, {219, -38}, {217, -40}, {215, 59}, {58, 57}, {56, 55}, {54, 53}, {52, 51}, {50, 49}, {48, 47}, {46, 45}, {44, 43}, {42, 41}, {40, -42}, {213, -44}, {211, -46}, {209, -48}, {207, -50}, {205, -52}, {203, -54}, {201, -56}, {199, -58}, {197, 63}, {62, 61}, {60, -60}, {195, -62}, {193, 0}, {0, -8}, {6, 63}, {0, -84}, {7, 125}, {0, 38}, {8, -45}, {128, 2}, {9, -7}, {64, 0}, {10, -2}, {0, 0}, {11, -1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}} }},
{{mpc_huff_Q8_1, mpc_sym_Q8_1, {{133, -124}, {131, -126}, {129, 126}, {125, 124}, {123, 122}, {121, 120}, {119, 118}, {117, 116}, {115, 114}, {113, 112}, {111, 110}, {109, 108}, {212, -45}, {210, -47}, {208, -49}, {206, -51}, {204, -53}, {202, -55}, {200, -57}, {198, -59}, {196, -61}, {194, -63}, {192, -65}, {190, -67}, {188, -69}, {186, -71}, {184, -73}, {182, -75}, {180, -77}, {178, -79}, {176, -81}, {174, -83}, {172, -85}, {170, -87}, {168, -89}, {166, -91}, {164, -93}, {162, -95}, {160, -97}, {158, -99}, {156, -101}, {154, -103}, {152, -105}, {150, -107}, {147, 107}, {106, 105}, {104, 103}, {102, 101}, {100, 99}, {98, 97}, {96, 95}, {94, 93}, {92, 91}, {90, 89}, {88, 87}, {86, 85}, {84, 83}, {82, 81}, {80, 79}, {78, 77}, {76, 75}, {74, 73}, {72, 71}, {70, 69}} }, {mpc_huff_Q8_2, mpc_sym_Q8_2, {{52, 51}, {50, 49}, {48, 47}, {46, 45}, {44, 43}, {42, 40}, {20, 19}, {249, -8}, {247, -10}, {245, -12}, {243, -14}, {241, -16}, {239, -18}, {237, -20}, {235, -22}, {233, -24}, {231, -26}, {229, -28}, {227, -30}, {225, -32}, {223, -34}, {221, -36}, {219, -38}, {217, -40}, {215, -42}, {213, 41}, {39, 38}, {37, 36}, {35, 34}, {33, 32}, {31, 30}, {29, 28}, {27, 26}, {25, 24}, {23, 22}, {21, 18}, {17, 16}, {15, 14}, {13, 12}, {11, 10}, {9, 8}, {7, 6}, {5, -3}, {252, -5}, {250, 4}, {3, 2}, {1, 0}, {255, -2}, {44, 32}, {0, 0}, {123, 37}, {117, 44}, {32, 37}, {100, 125}, {37, 115}, {0, 0}, {109, 112}, {99, 95}, {108, 117}, {116, 95}, {100, 97}, {116, 97}, {32, 109}, {112, 99}} }}
};
