#pragma once
#include <cstdarg>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class SVM {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        float kernels[501] = { 0 };
                        float decisions[10] = { 0 };
                        int votes[5] = { 0 };
                        kernels[0] = compute_kernel(x,   3.5  , 1.3  , 1.0  , 84.23  , 1027.9  , 2.0 );
                        kernels[1] = compute_kernel(x,   2.5  , 1.0  , 0.8  , 89.16  , 1041.9  , 2.0 );
                        kernels[2] = compute_kernel(x,   3.3  , 2.7  , 1.9  , 91.12  , 1031.6  , 3.0 );
                        kernels[3] = compute_kernel(x,   7.7  , 6.4  , 2.6  , 72.86  , 1021.1  , 7.0 );
                        kernels[4] = compute_kernel(x,   6.9  , 4.8  , 0.3  , 63.75  , 1026.5  , 7.0 );
                        kernels[5] = compute_kernel(x,   7.1  , 5.1  , -2.0  , 54.19  , 1026.6  , 7.0 );
                        kernels[6] = compute_kernel(x,   8.0  , 6.7  , -1.0  , 55.33  , 1025.7  , 7.0 );
                        kernels[7] = compute_kernel(x,   7.4  , 6.8  , -3.1  , 50.62  , 1024.5  , 7.0 );
                        kernels[8] = compute_kernel(x,   7.2  , 6.1  , -2.5  , 54.15  , 1023.3  , 7.0 );
                        kernels[9] = compute_kernel(x,   7.6  , 6.2  , 0.0  , 61.77  , 1023.9  , 6.0 );
                        kernels[10] = compute_kernel(x,   13.9  , 12.9  , 3.3  , 51.63  , 1014.0  , 7.0 );
                        kernels[11] = compute_kernel(x,   12.3  , 12.2  , 4.3  , 60.68  , 1028.4  , 7.0 );
                        kernels[12] = compute_kernel(x,   15.3  , 15.2  , 7.4  , 62.01  , 1024.6  , 7.0 );
                        kernels[13] = compute_kernel(x,   15.4  , 15.2  , 7.2  , 61.05  , 1024.6  , 7.0 );
                        kernels[14] = compute_kernel(x,   10.5  , 10.1  , 1.4  , 57.16  , 1022.7  , 7.0 );
                        kernels[15] = compute_kernel(x,   12.1  , 11.2  , 2.5  , 55.57  , 1017.5  , 8.0 );
                        kernels[16] = compute_kernel(x,   13.4  , 13.1  , 2.7  , 50.14  , 1016.6  , 8.0 );
                        kernels[17] = compute_kernel(x,   14.2  , 13.9  , 3.8  , 53.62  , 1019.3  , 8.0 );
                        kernels[18] = compute_kernel(x,   15.0  , 14.7  , 5.2  , 56.35  , 1019.4  , 8.0 );
                        kernels[19] = compute_kernel(x,   14.8  , 14.7  , 4.8  , 53.85  , 1016.1  , 8.0 );
                        kernels[20] = compute_kernel(x,   12.6  , 12.4  , 4.3  , 61.32  , 1012.2  , 8.0 );
                        kernels[21] = compute_kernel(x,   11.7  , 10.8  , 3.3  , 59.8  , 1025.2  , 8.0 );
                        kernels[22] = compute_kernel(x,   14.8  , 14.6  , 6.7  , 62.27  , 1022.8  , 8.0 );
                        kernels[23] = compute_kernel(x,   18.0  , 18.0  , 6.4  , 50.36  , 1012.6  , 8.0 );
                        kernels[24] = compute_kernel(x,   11.3  , 11.1  , 2.5  , 57.71  , 1024.0  , 10.0 );
                        kernels[25] = compute_kernel(x,   20.5  , 20.4  , 8.6  , 50.62  , 1021.3  , 8.0 );
                        kernels[26] = compute_kernel(x,   20.3  , 20.2  , 8.2  , 48.26  , 1018.9  , 8.0 );
                        kernels[27] = compute_kernel(x,   16.9  , 16.9  , 5.3  , 50.41  , 1033.9  , 8.0 );
                        kernels[28] = compute_kernel(x,   17.8  , 17.8  , 5.5  , 49.06  , 1023.2  , 8.0 );
                        kernels[29] = compute_kernel(x,   18.8  , 18.8  , 5.7  , 44.95  , 1022.4  , 8.0 );
                        kernels[30] = compute_kernel(x,   19.1  , 19.0  , 7.5  , 50.49  , 1022.4  , 8.0 );
                        kernels[31] = compute_kernel(x,   19.2  , 19.2  , 6.2  , 46.55  , 1019.0  , 8.0 );
                        kernels[32] = compute_kernel(x,   17.3  , 17.3  , 8.8  , 60.31  , 1024.6  , 9.0 );
                        kernels[33] = compute_kernel(x,   20.8  , 20.8  , 11.6  , 59.0  , 1023.5  , 8.0 );
                        kernels[34] = compute_kernel(x,   24.5  , 24.2  , 12.9  , 51.15  , 1020.0  , 8.0 );
                        kernels[35] = compute_kernel(x,   25.8  , 25.4  , 13.1  , 48.09  , 1015.3  , 8.0 );
                        kernels[36] = compute_kernel(x,   18.1  , 18.1  , 7.4  , 53.43  , 1027.8  , 8.0 );
                        kernels[37] = compute_kernel(x,   16.9  , 16.8  , 6.0  , 51.5  , 1022.9  , 9.0 );
                        kernels[38] = compute_kernel(x,   17.2  , 17.2  , 5.6  , 49.17  , 1025.9  , 9.0 );
                        kernels[39] = compute_kernel(x,   21.6  , 21.2  , 8.6  , 47.9  , 1018.1  , 8.0 );
                        kernels[40] = compute_kernel(x,   14.1  , 14.1  , 6.2  , 63.48  , 1020.8  , 7.0 );
                        kernels[41] = compute_kernel(x,   18.3  , 18.2  , 12.5  , 72.36  , 1023.3  , 6.0 );
                        kernels[42] = compute_kernel(x,   19.9  , 19.6  , 11.8  , 65.96  , 1021.6  , 7.0 );
                        kernels[43] = compute_kernel(x,   15.9  , 15.8  , 8.4  , 63.75  , 1022.9  , 6.0 );
                        kernels[44] = compute_kernel(x,   18.2  , 18.1  , 9.5  , 60.98  , 1017.5  , 6.0 );
                        kernels[45] = compute_kernel(x,   18.6  , 18.5  , 11.2  , 65.3  , 1018.5  , 6.0 );
                        kernels[46] = compute_kernel(x,   5.7  , 5.4  , 3.0  , 84.25  , 1033.5  , 4.0 );
                        kernels[47] = compute_kernel(x,   9.3  , 8.9  , 6.9  , 85.54  , 1020.2  , 2.0 );
                        kernels[48] = compute_kernel(x,   6.3  , 5.9  , 0.7  , 69.66  , 1037.3  , 5.0 );
                        kernels[49] = compute_kernel(x,   13.4  , 13.2  , 3.9  , 59.64  , 1027.6  , 7.0 );
                        kernels[50] = compute_kernel(x,   7.6  , 6.9  , -2.9  , 50.76  , 1029.1  , 8.0 );
                        kernels[51] = compute_kernel(x,   10.6  , 10.4  , 2.1  , 59.9  , 1021.4  , 8.0 );
                        kernels[52] = compute_kernel(x,   11.3  , 11.2  , 3.6  , 61.29  , 1018.0  , 8.0 );
                        kernels[53] = compute_kernel(x,   8.3  , 6.7  , -0.3  , 57.36  , 1027.1  , 8.0 );
                        kernels[54] = compute_kernel(x,   9.7  , 9.0  , 1.1  , 59.38  , 1028.5  , 8.0 );
                        kernels[55] = compute_kernel(x,   10.7  , 10.0  , 1.2  , 54.83  , 1027.5  , 8.0 );
                        kernels[56] = compute_kernel(x,   19.0  , 18.9  , 8.2  , 52.82  , 1015.2  , 9.0 );
                        kernels[57] = compute_kernel(x,   18.7  , 18.6  , 10.9  , 63.82  , 1023.2  , 9.0 );
                        kernels[58] = compute_kernel(x,   19.2  , 19.2  , 9.9  , 60.56  , 1021.7  , 9.0 );
                        kernels[59] = compute_kernel(x,   21.6  , 21.5  , 11.9  , 55.75  , 1026.0  , 8.0 );
                        kernels[60] = compute_kernel(x,   19.5  , 19.5  , 11.9  , 64.36  , 1026.6  , 8.0 );
                        kernels[61] = compute_kernel(x,   22.4  , 22.3  , 13.3  , 58.23  , 1028.0  , 8.0 );
                        kernels[62] = compute_kernel(x,   24.5  , 24.5  , 15.0  , 57.94  , 1024.8  , 8.0 );
                        kernels[63] = compute_kernel(x,   25.0  , 25.0  , 16.5  , 61.89  , 1020.7  , 8.0 );
                        kernels[64] = compute_kernel(x,   24.3  , 24.2  , 16.2  , 63.96  , 1019.6  , 8.0 );
                        kernels[65] = compute_kernel(x,   24.5  , 24.4  , 15.8  , 60.96  , 1021.9  , 8.0 );
                        kernels[66] = compute_kernel(x,   23.7  , 23.7  , 15.1  , 60.4  , 1022.5  , 8.0 );
                        kernels[67] = compute_kernel(x,   19.9  , 19.8  , 13.5  , 68.01  , 1017.1  , 8.0 );
                        kernels[68] = compute_kernel(x,   21.0  , 20.9  , 14.9  , 71.04  , 1023.0  , 5.0 );
                        kernels[69] = compute_kernel(x,   22.3  , 22.2  , 14.7  , 66.05  , 1020.9  , 6.0 );
                        kernels[70] = compute_kernel(x,   22.3  , 22.1  , 13.6  , 61.39  , 1013.0  , 7.0 );
                        kernels[71] = compute_kernel(x,   16.3  , 16.3  , 12.6  , 80.84  , 1017.5  , 6.0 );
                        kernels[72] = compute_kernel(x,   18.3  , 18.2  , 12.5  , 71.89  , 1013.9  , 6.0 );
                        kernels[73] = compute_kernel(x,   15.5  , 15.5  , 11.0  , 76.47  , 1030.1  , 6.0 );
                        kernels[74] = compute_kernel(x,   15.2  , 15.2  , 11.3  , 79.84  , 1027.7  , 6.0 );
                        kernels[75] = compute_kernel(x,   12.9  , 12.8  , 7.3  , 69.72  , 1018.2  , 6.0 );
                        kernels[76] = compute_kernel(x,   11.4  , 11.3  , 7.4  , 77.22  , 1030.6  , 5.0 );
                        kernels[77] = compute_kernel(x,   12.5  , 12.5  , 10.7  , 89.81  , 1017.4  , 3.0 );
                        kernels[78] = compute_kernel(x,   10.2  , 9.6  , 5.3  , 72.05  , 994.4  , 4.0 );
                        kernels[79] = compute_kernel(x,   10.2  , 9.3  , 4.7  , 69.47  , 1021.2  , 3.0 );
                        kernels[80] = compute_kernel(x,   5.7  , 3.6  , 2.6  , 81.33  , 1030.1  , 3.0 );
                        kernels[81] = compute_kernel(x,   1.6  , -1.6  , -3.8  , 68.17  , 1006.9  , 2.0 );
                        kernels[82] = compute_kernel(x,   8.3  , 6.4  , 5.2  , 81.85  , 1021.4  , 1.0 );
                        kernels[83] = compute_kernel(x,   7.6  , 4.6  , 4.6  , 81.65  , 1022.2  , 1.0 );
                        kernels[84] = compute_kernel(x,   11.3  , 11.2  , 9.2  , 86.99  , 1020.6  , 1.0 );
                        kernels[85] = compute_kernel(x,   6.4  , 3.7  , 2.6  , 77.26  , 1023.1  , 1.0 );
                        kernels[86] = compute_kernel(x,   9.4  , 7.9  , 6.5  , 82.23  , 1018.2  , 2.0 );
                        kernels[87] = compute_kernel(x,   2.4  , 1.4  , 0.0  , 85.29  , 1044.3  , 2.0 );
                        kernels[88] = compute_kernel(x,   5.5  , 4.9  , 4.8  , 95.51  , 1037.6  , 1.0 );
                        kernels[89] = compute_kernel(x,   9.2  , 6.8  , 4.9  , 75.16  , 1012.6  , 2.0 );
                        kernels[90] = compute_kernel(x,   6.9  , 3.6  , 2.3  , 72.75  , 1022.1  , 1.0 );
                        kernels[91] = compute_kernel(x,   5.7  , 3.1  , 3.4  , 86.11  , 1019.4  , 3.0 );
                        kernels[92] = compute_kernel(x,   9.2  , 7.2  , 5.3  , 77.7  , 1014.6  , 3.0 );
                        kernels[93] = compute_kernel(x,   5.2  , 0.2  , -1.3  , 63.57  , 1007.9  , 4.0 );
                        kernels[94] = compute_kernel(x,   7.8  , 4.3  , 2.7  , 70.45  , 1009.6  , 3.0 );
                        kernels[95] = compute_kernel(x,   6.9  , 4.3  , 3.1  , 77.59  , 1018.1  , 3.0 );
                        kernels[96] = compute_kernel(x,   6.4  , 3.3  , 3.8  , 83.77  , 1018.7  , 1.0 );
                        kernels[97] = compute_kernel(x,   10.8  , 9.4  , 6.0  , 72.55  , 1018.3  , 4.0 );
                        kernels[98] = compute_kernel(x,   10.9  , 10.1  , 6.6  , 75.77  , 1017.0  , 2.0 );
                        kernels[99] = compute_kernel(x,   4.6  , 0.6  , -0.4  , 70.9  , 1003.9  , 4.0 );
                        kernels[100] = compute_kernel(x,   4.4  , 1.1  , 0.1  , 75.31  , 1003.8  , 3.0 );
                        kernels[101] = compute_kernel(x,   5.9  , 3.9  , 0.7  , 71.42  , 1005.3  , 6.0 );
                        kernels[102] = compute_kernel(x,   7.4  , 3.2  , 1.4  , 66.25  , 1010.1  , 6.0 );
                        kernels[103] = compute_kernel(x,   8.4  , 5.8  , 2.5  , 67.52  , 1017.4  , 5.0 );
                        kernels[104] = compute_kernel(x,   7.1  , 4.6  , 6.2  , 94.35  , 1026.2  , 1.0 );
                        kernels[105] = compute_kernel(x,   7.3  , 4.1  , 2.0  , 69.73  , 1028.5  , 4.0 );
                        kernels[106] = compute_kernel(x,   13.5  , 13.5  , 6.1  , 62.78  , 1016.7  , 6.0 );
                        kernels[107] = compute_kernel(x,   16.7  , 16.7  , 6.9  , 55.89  , 1014.7  , 8.0 );
                        kernels[108] = compute_kernel(x,   11.1  , 10.5  , 5.8  , 70.45  , 1016.7  , 4.0 );
                        kernels[109] = compute_kernel(x,   9.9  , 8.9  , 6.5  , 80.5  , 1017.7  , 5.0 );
                        kernels[110] = compute_kernel(x,   11.9  , 11.7  , 6.3  , 70.12  , 1015.8  , 4.0 );
                        kernels[111] = compute_kernel(x,   14.9  , 14.8  , 5.6  , 56.3  , 1022.2  , 9.0 );
                        kernels[112] = compute_kernel(x,   16.0  , 16.0  , 8.5  , 61.6  , 1008.8  , 3.0 );
                        kernels[113] = compute_kernel(x,   12.8  , 12.8  , 6.5  , 67.15  , 1003.8  , 7.0 );
                        kernels[114] = compute_kernel(x,   13.3  , 13.2  , 10.3  , 82.91  , 1015.4  , 2.0 );
                        kernels[115] = compute_kernel(x,   18.0  , 18.0  , 11.4  , 69.77  , 1008.1  , 8.0 );
                        kernels[116] = compute_kernel(x,   18.1  , 18.0  , 12.2  , 70.48  , 1013.8  , 7.0 );
                        kernels[117] = compute_kernel(x,   17.6  , 17.5  , 11.9  , 72.25  , 1016.1  , 10.0 );
                        kernels[118] = compute_kernel(x,   18.2  , 18.2  , 11.9  , 69.06  , 1014.2  , 8.0 );
                        kernels[119] = compute_kernel(x,   17.6  , 17.6  , 12.7  , 74.3  , 1012.4  , 6.0 );
                        kernels[120] = compute_kernel(x,   15.7  , 15.7  , 12.2  , 80.71  , 1014.0  , 5.0 );
                        kernels[121] = compute_kernel(x,   17.0  , 17.0  , 11.8  , 73.1  , 1019.3  , 6.0 );
                        kernels[122] = compute_kernel(x,   17.8  , 17.8  , 11.3  , 67.94  , 1016.8  , 7.0 );
                        kernels[123] = compute_kernel(x,   16.3  , 16.2  , 9.1  , 63.87  , 1009.3  , 6.0 );
                        kernels[124] = compute_kernel(x,   15.7  , 15.6  , 7.8  , 60.99  , 1009.9  , 5.0 );
                        kernels[125] = compute_kernel(x,   16.6  , 16.5  , 11.9  , 75.05  , 1011.1  , 6.0 );
                        kernels[126] = compute_kernel(x,   16.7  , 16.6  , 11.1  , 72.05  , 1015.8  , 7.0 );
                        kernels[127] = compute_kernel(x,   15.5  , 15.5  , 8.8  , 67.39  , 1017.5  , 8.0 );
                        kernels[128] = compute_kernel(x,   17.9  , 17.9  , 12.3  , 71.84  , 1016.5  , 5.0 );
                        kernels[129] = compute_kernel(x,   17.6  , 17.6  , 11.0  , 67.76  , 1017.8  , 4.0 );
                        kernels[130] = compute_kernel(x,   16.7  , 16.6  , 8.5  , 61.1  , 1013.9  , 6.0 );
                        kernels[131] = compute_kernel(x,   27.0  , 27.2  , 17.0  , 56.28  , 1013.0  , 7.0 );
                        kernels[132] = compute_kernel(x,   22.6  , 22.7  , 17.8  , 74.63  , 1012.3  , 7.0 );
                        kernels[133] = compute_kernel(x,   20.7  , 20.7  , 16.6  , 78.87  , 1010.8  , 5.0 );
                        kernels[134] = compute_kernel(x,   19.8  , 19.8  , 14.7  , 73.51  , 1008.9  , 4.0 );
                        kernels[135] = compute_kernel(x,   19.2  , 19.1  , 11.9  , 64.24  , 1011.6  , 6.0 );
                        kernels[136] = compute_kernel(x,   17.2  , 17.1  , 12.8  , 76.49  , 1015.4  , 6.0 );
                        kernels[137] = compute_kernel(x,   13.9  , 13.8  , 7.8  , 67.56  , 1017.3  , 4.0 );
                        kernels[138] = compute_kernel(x,   14.7  , 14.7  , 10.1  , 75.39  , 1019.1  , 6.0 );
                        kernels[139] = compute_kernel(x,   18.5  , 18.4  , 15.6  , 83.67  , 1015.8  , 2.0 );
                        kernels[140] = compute_kernel(x,   14.7  , 14.6  , 8.3  , 68.84  , 1021.1  , 6.0 );
                        kernels[141] = compute_kernel(x,   14.5  , 14.5  , 8.1  , 67.36  , 1017.4  , 6.0 );
                        kernels[142] = compute_kernel(x,   15.9  , 15.8  , 12.3  , 81.18  , 1015.0  , 4.0 );
                        kernels[143] = compute_kernel(x,   9.8  , 9.6  , 7.3  , 85.59  , 1018.8  , 4.0 );
                        kernels[144] = compute_kernel(x,   9.7  , 9.6  , 7.6  , 87.68  , 1019.9  , 2.0 );
                        kernels[145] = compute_kernel(x,   10.7  , 9.7  , 7.6  , 82.15  , 1020.2  , 4.0 );
                        kernels[146] = compute_kernel(x,   9.8  , 8.6  , 6.8  , 81.86  , 1024.5  , 5.0 );
                        kernels[147] = compute_kernel(x,   7.7  , 5.3  , 4.2  , 78.85  , 1018.9  , 2.0 );
                        kernels[148] = compute_kernel(x,   4.8  , 4.6  , 3.8  , 93.9  , 1038.6  , 3.0 );
                        kernels[149] = compute_kernel(x,   6.6  , 5.4  , 3.6  , 82.76  , 1029.1  , 2.0 );
                        kernels[150] = compute_kernel(x,   12.0  , 11.9  , 10.7  , 92.34  , 1023.1  , 2.0 );
                        kernels[151] = compute_kernel(x,   11.4  , 11.1  , 9.8  , 90.53  , 1017.3  , 1.0 );
                        kernels[152] = compute_kernel(x,   10.7  , 9.7  , 7.4  , 80.91  , 1013.4  , 3.0 );
                        kernels[153] = compute_kernel(x,   10.7  , 10.0  , 7.5  , 80.94  , 1012.0  , 2.0 );
                        kernels[154] = compute_kernel(x,   12.9  , 12.9  , 10.4  , 84.8  , 1020.5  , 1.0 );
                        kernels[155] = compute_kernel(x,   11.8  , 11.2  , 8.8  , 81.94  , 1016.5  , 2.0 );
                        kernels[156] = compute_kernel(x,   7.9  , 5.4  , 4.0  , 76.32  , 1026.2  , 1.0 );
                        kernels[157] = compute_kernel(x,   6.2  , 4.9  , 5.0  , 92.47  , 1033.1  , 1.0 );
                        kernels[158] = compute_kernel(x,   9.4  , 8.8  , 6.9  , 84.44  , 1026.6  , 2.0 );
                        kernels[159] = compute_kernel(x,   3.0  , 2.1  , 1.8  , 92.46  , 1019.2  , 1.0 );
                        kernels[160] = compute_kernel(x,   4.9  , 3.4  , 3.0  , 87.99  , 1018.0  , 1.0 );
                        kernels[161] = compute_kernel(x,   4.2  , 0.5  , 1.9  , 85.3  , 977.3  , 1.0 );
                        kernels[162] = compute_kernel(x,   1.7  , 0.9  , 1.2  , 96.18  , 1003.4  , 1.0 );
                        kernels[163] = compute_kernel(x,   4.7  , 1.0  , 1.2  , 77.83  , 1020.9  , 1.0 );
                        kernels[164] = compute_kernel(x,   1.3  , -1.1  , 0.3  , 93.21  , 974.6  , 1.0 );
                        kernels[165] = compute_kernel(x,   1.5  , -0.7  , 0.6  , 94.31  , 1011.7  , 1.0 );
                        kernels[166] = compute_kernel(x,   3.3  , 1.3  , 1.0  , 85.48  , 1016.0  , 1.0 );
                        kernels[167] = compute_kernel(x,   3.8  , -0.2  , 1.0  , 81.71  , 1019.5  , 1.0 );
                        kernels[168] = compute_kernel(x,   3.3  , -0.5  , 0.6  , 82.12  , 1020.1  , 1.0 );
                        kernels[169] = compute_kernel(x,   3.4  , 0.7  , 1.0  , 84.71  , 1019.5  , 1.0 );
                        kernels[170] = compute_kernel(x,   6.0  , 4.4  , 5.4  , 95.79  , 1018.4  , 0.0 );
                        kernels[171] = compute_kernel(x,   5.9  , 3.5  , 5.2  , 95.29  , 1016.7  , 0.0 );
                        kernels[172] = compute_kernel(x,   1.3  , -0.6  , -0.1  , 90.85  , 1029.8  , 1.0 );
                        kernels[173] = compute_kernel(x,   5.1  , 1.9  , 3.0  , 86.97  , 1020.0  , 1.0 );
                        kernels[174] = compute_kernel(x,   4.7  , 2.0  , 3.2  , 90.31  , 1021.3  , 1.0 );
                        kernels[175] = compute_kernel(x,   1.4  , -0.8  , -0.2  , 89.22  , 995.7  , 2.0 );
                        kernels[176] = compute_kernel(x,   1.0  , -2.5  , -2.0  , 80.98  , 1009.7  , 3.0 );
                        kernels[177] = compute_kernel(x,   2.1  , 0.1  , 0.9  , 91.78  , 1015.8  , 1.0 );
                        kernels[178] = compute_kernel(x,   2.5  , -0.9  , -0.3  , 82.23  , 997.9  , 1.0 );
                        kernels[179] = compute_kernel(x,   1.9  , -3.1  , -1.3  , 79.81  , 1004.6  , 2.0 );
                        kernels[180] = compute_kernel(x,   2.6  , -2.1  , -3.8  , 63.64  , 1025.6  , 2.0 );
                        kernels[181] = compute_kernel(x,   6.4  , 6.1  , 1.8  , 74.38  , 1040.9  , 6.0 );
                        kernels[182] = compute_kernel(x,   5.8  , 4.3  , 4.9  , 94.02  , 1027.6  , 2.0 );
                        kernels[183] = compute_kernel(x,   5.1  , 2.8  , 2.8  , 85.28  , 1025.1  , 1.0 );
                        kernels[184] = compute_kernel(x,   7.7  , 4.3  , 3.3  , 74.7  , 1013.0  , 3.0 );
                        kernels[185] = compute_kernel(x,   9.5  , 7.9  , 4.0  , 68.58  , 1020.6  , 4.0 );
                        kernels[186] = compute_kernel(x,   7.3  , 5.6  , 3.0  , 74.64  , 1033.3  , 4.0 );
                        kernels[187] = compute_kernel(x,   8.2  , 6.4  , 3.2  , 72.83  , 1028.2  , 7.0 );
                        kernels[188] = compute_kernel(x,   7.4  , 4.6  , 2.0  , 70.37  , 1020.6  , 6.0 );
                        kernels[189] = compute_kernel(x,   4.7  , 2.4  , -2.2  , 62.54  , 1019.2  , 5.0 );
                        kernels[190] = compute_kernel(x,   4.4  , 2.9  , -2.2  , 67.54  , 1028.6  , 8.0 );
                        kernels[191] = compute_kernel(x,   8.5  , 7.2  , 2.6  , 67.42  , 1006.5  , 2.0 );
                        kernels[192] = compute_kernel(x,   7.7  , 6.2  , 0.3  , 61.64  , 1011.3  , 5.0 );
                        kernels[193] = compute_kernel(x,   9.4  , 7.8  , 3.7  , 69.33  , 1013.1  , 5.0 );
                        kernels[194] = compute_kernel(x,   7.2  , 5.2  , 1.7  , 70.09  , 1008.7  , 10.0 );
                        kernels[195] = compute_kernel(x,   7.1  , 6.3  , 0.4  , 66.75  , 1007.3  , 6.0 );
                        kernels[196] = compute_kernel(x,   11.9  , 11.5  , 5.9  , 69.37  , 1007.0  , 8.0 );
                        kernels[197] = compute_kernel(x,   12.8  , 12.4  , 5.4  , 62.76  , 1008.5  , 6.0 );
                        kernels[198] = compute_kernel(x,   12.4  , 12.1  , 6.5  , 69.85  , 1017.5  , 9.0 );
                        kernels[199] = compute_kernel(x,   10.7  , 9.8  , 6.2  , 75.51  , 1013.5  , 3.0 );
                        kernels[200] = compute_kernel(x,   14.9  , 14.9  , 9.3  , 70.23  , 1023.2  , 6.0 );
                        kernels[201] = compute_kernel(x,   19.2  , 19.2  , 13.5  , 70.57  , 1018.2  , 9.0 );
                        kernels[202] = compute_kernel(x,   14.7  , 14.7  , 12.0  , 83.76  , 1022.8  , 2.0 );
                        kernels[203] = compute_kernel(x,   17.7  , 17.7  , 12.6  , 73.27  , 1024.5  , 6.0 );
                        kernels[204] = compute_kernel(x,   17.6  , 17.6  , 11.3  , 68.01  , 1016.3  , 7.0 );
                        kernels[205] = compute_kernel(x,   15.9  , 15.8  , 12.8  , 82.79  , 1015.3  , 5.0 );
                        kernels[206] = compute_kernel(x,   18.5  , 18.5  , 14.5  , 78.27  , 1011.9  , 4.0 );
                        kernels[207] = compute_kernel(x,   17.2  , 17.2  , 13.6  , 79.78  , 1019.3  , 4.0 );
                        kernels[208] = compute_kernel(x,   16.0  , 16.0  , 11.6  , 75.22  , 1016.7  , 3.0 );
                        kernels[209] = compute_kernel(x,   18.5  , 18.5  , 14.1  , 76.75  , 1014.6  , 7.0 );
                        kernels[210] = compute_kernel(x,   18.9  , 18.9  , 14.3  , 74.81  , 1008.3  , 5.0 );
                        kernels[211] = compute_kernel(x,   16.5  , 16.4  , 11.2  , 73.4  , 1013.1  , 8.0 );
                        kernels[212] = compute_kernel(x,   17.6  , 17.6  , 13.2  , 77.47  , 1015.3  , 8.0 );
                        kernels[213] = compute_kernel(x,   18.3  , 18.2  , 14.9  , 81.34  , 1018.2  , 4.0 );
                        kernels[214] = compute_kernel(x,   18.5  , 18.4  , 13.9  , 76.19  , 1016.9  , 8.0 );
                        kernels[215] = compute_kernel(x,   17.6  , 17.6  , 14.2  , 81.21  , 1016.0  , 3.0 );
                        kernels[216] = compute_kernel(x,   17.7  , 17.7  , 14.9  , 83.89  , 1015.5  , 2.0 );
                        kernels[217] = compute_kernel(x,   18.6  , 18.6  , 14.6  , 78.36  , 1018.6  , 7.0 );
                        kernels[218] = compute_kernel(x,   16.1  , 16.1  , 10.8  , 72.65  , 1010.4  , 8.0 );
                        kernels[219] = compute_kernel(x,   13.6  , 13.6  , 10.6  , 82.27  , 1015.8  , 6.0 );
                        kernels[220] = compute_kernel(x,   12.0  , 11.5  , 6.7  , 71.97  , 1019.1  , 7.0 );
                        kernels[221] = compute_kernel(x,   12.6  , 12.2  , 9.5  , 81.83  , 1020.7  , 2.0 );
                        kernels[222] = compute_kernel(x,   14.8  , 14.8  , 10.7  , 78.01  , 1010.1  , 3.0 );
                        kernels[223] = compute_kernel(x,   15.4  , 15.4  , 13.6  , 89.46  , 1029.5  , 2.0 );
                        kernels[224] = compute_kernel(x,   12.5  , 12.4  , 10.0  , 86.17  , 1028.7  , 5.0 );
                        kernels[225] = compute_kernel(x,   11.8  , 11.7  , 9.6  , 86.88  , 1020.5  , 5.0 );
                        kernels[226] = compute_kernel(x,   9.5  , 8.0  , 4.3  , 71.19  , 1007.0  , 5.0 );
                        kernels[227] = compute_kernel(x,   12.8  , 12.8  , 8.2  , 74.11  , 1018.1  , 5.0 );
                        kernels[228] = compute_kernel(x,   11.8  , 11.7  , 10.9  , 94.16  , 1022.9  , 1.0 );
                        kernels[229] = compute_kernel(x,   11.9  , 11.9  , 8.2  , 78.26  , 1015.8  , 1.0 );
                        kernels[230] = compute_kernel(x,   8.5  , 7.3  , 6.0  , 85.02  , 1024.2  , 3.0 );
                        kernels[231] = compute_kernel(x,   4.8  , 4.5  , 2.8  , 87.85  , 1032.4  , 2.0 );
                        kernels[232] = compute_kernel(x,   2.5  , -0.7  , -1.9  , 72.89  , 1011.8  , 2.0 );
                        kernels[233] = compute_kernel(x,   11.9  , 11.9  , 10.2  , 89.7  , 1019.5  , 1.0 );
                        kernels[234] = compute_kernel(x,   2.0  , -0.4  , -0.5  , 83.69  , 1019.2  , 1.0 );
                        kernels[235] = compute_kernel(x,   13.6  , 13.6  , 11.0  , 84.72  , 1016.5  , 1.0 );
                        kernels[236] = compute_kernel(x,   6.3  , 3.6  , 3.2  , 80.82  , 1034.2  , 1.0 );
                        kernels[237] = compute_kernel(x,   9.3  , 7.5  , 6.2  , 81.14  , 1024.3  , 1.0 );
                        kernels[238] = compute_kernel(x,   3.3  , 2.3  , 0.9  , 84.54  , 1047.7  , 2.0 );
                        kernels[239] = compute_kernel(x,   5.9  , 2.2  , 2.3  , 77.78  , 1009.0  , 3.0 );
                        kernels[240] = compute_kernel(x,   10.6  , 9.5  , 9.0  , 89.41  , 1006.1  , 1.0 );
                        kernels[241] = compute_kernel(x,   5.1  , 4.6  , 3.2  , 87.74  , 1036.6  , 3.0 );
                        kernels[242] = compute_kernel(x,   5.4  , 1.3  , 1.1  , 74.77  , 1014.2  , 4.0 );
                        kernels[243] = compute_kernel(x,   7.7  , 6.5  , 5.1  , 84.18  , 1016.5  , 3.0 );
                        kernels[244] = compute_kernel(x,   7.7  , 4.8  , 3.3  , 73.72  , 1022.4  , 2.0 );
                        kernels[245] = compute_kernel(x,   6.6  , 2.7  , 0.9  , 68.08  , 988.8  , 6.0 );
                        kernels[246] = compute_kernel(x,   7.9  , 6.6  , 4.9  , 81.73  , 1015.0  , 3.0 );
                        kernels[247] = compute_kernel(x,   11.0  , 10.1  , 7.0  , 76.96  , 1008.9  , 6.0 );
                        kernels[248] = compute_kernel(x,   9.1  , 7.7  , 6.1  , 81.86  , 1027.2  , 3.0 );
                        kernels[249] = compute_kernel(x,   5.2  , 1.5  , -3.5  , 54.34  , 1037.6  , 4.0 );
                        kernels[250] = compute_kernel(x,   9.4  , 8.3  , 3.2  , 66.02  , 1016.3  , 4.0 );
                        kernels[251] = compute_kernel(x,   9.7  , 9.1  , 3.3  , 66.37  , 1017.6  , 5.0 );
                        kernels[252] = compute_kernel(x,   9.6  , 9.0  , 2.7  , 66.15  , 1020.1  , 7.0 );
                        kernels[253] = compute_kernel(x,   15.1  , 15.1  , 7.5  , 61.7  , 1024.7  , 5.0 );
                        kernels[254] = compute_kernel(x,   16.8  , 16.8  , 7.3  , 57.68  , 1022.1  , 7.0 );
                        kernels[255] = compute_kernel(x,   9.2  , 7.2  , 1.3  , 59.3  , 1024.8  , 8.0 );
                        kernels[256] = compute_kernel(x,   8.0  , 6.4  , -1.1  , 53.44  , 1028.6  , 8.0 );
                        kernels[257] = compute_kernel(x,   12.6  , 12.3  , 4.9  , 62.17  , 1016.2  , 5.0 );
                        kernels[258] = compute_kernel(x,   11.8  , 11.3  , 4.0  , 63.83  , 1020.1  , 8.0 );
                        kernels[259] = compute_kernel(x,   11.3  , 10.5  , 5.2  , 67.72  , 1015.2  , 8.0 );
                        kernels[260] = compute_kernel(x,   12.8  , 12.5  , 6.5  , 66.06  , 1007.6  , 5.0 );
                        kernels[261] = compute_kernel(x,   11.7  , 10.7  , 4.1  , 62.53  , 1010.3  , 8.0 );
                        kernels[262] = compute_kernel(x,   11.1  , 10.0  , 2.9  , 59.34  , 1021.1  , 8.0 );
                        kernels[263] = compute_kernel(x,   17.6  , 17.6  , 7.0  , 51.87  , 1017.9  , 8.0 );
                        kernels[264] = compute_kernel(x,   12.5  , 11.4  , 5.9  , 64.53  , 1010.6  , 6.0 );
                        kernels[265] = compute_kernel(x,   8.3  , 6.0  , -1.7  , 50.36  , 1019.6  , 7.0 );
                        kernels[266] = compute_kernel(x,   9.2  , 8.9  , -0.2  , 55.01  , 1021.0  , 8.0 );
                        kernels[267] = compute_kernel(x,   9.5  , 8.4  , 0.3  , 54.84  , 1017.5  , 5.0 );
                        kernels[268] = compute_kernel(x,   8.9  , 7.8  , -0.1  , 56.48  , 1022.7  , 8.0 );
                        kernels[269] = compute_kernel(x,   12.8  , 12.7  , 3.2  , 53.91  , 1024.4  , 6.0 );
                        kernels[270] = compute_kernel(x,   14.4  , 14.1  , 5.6  , 57.98  , 1023.9  , 8.0 );
                        kernels[271] = compute_kernel(x,   16.7  , 16.7  , 6.4  , 54.58  , 1023.6  , 8.0 );
                        kernels[272] = compute_kernel(x,   18.7  , 18.6  , 10.0  , 59.61  , 1023.2  , 8.0 );
                        kernels[273] = compute_kernel(x,   18.8  , 18.7  , 7.6  , 49.89  , 1015.1  , 8.0 );
                        kernels[274] = compute_kernel(x,   16.3  , 16.3  , 7.7  , 58.94  , 1030.1  , 8.0 );
                        kernels[275] = compute_kernel(x,   20.0  , 20.0  , 9.4  , 51.68  , 1035.1  , 8.0 );
                        kernels[276] = compute_kernel(x,   15.1  , 15.1  , 6.0  , 55.81  , 1020.1  , 7.0 );
                        kernels[277] = compute_kernel(x,   23.5  , 23.5  , 14.3  , 57.95  , 1009.0  , 8.0 );
                        kernels[278] = compute_kernel(x,   18.5  , 18.5  , 10.0  , 60.75  , 1010.8  , 9.0 );
                        kernels[279] = compute_kernel(x,   15.9  , 15.8  , 7.6  , 59.39  , 1019.0  , 4.0 );
                        kernels[280] = compute_kernel(x,   15.9  , 15.9  , 8.7  , 64.75  , 1020.3  , 5.0 );
                        kernels[281] = compute_kernel(x,   15.8  , 15.6  , 6.5  , 56.66  , 1026.3  , 7.0 );
                        kernels[282] = compute_kernel(x,   19.2  , 19.1  , 9.1  , 54.99  , 1023.2  , 8.0 );
                        kernels[283] = compute_kernel(x,   16.5  , 16.5  , 10.7  , 68.79  , 1016.1  , 3.0 );
                        kernels[284] = compute_kernel(x,   22.4  , 22.3  , 12.8  , 57.3  , 1020.6  , 8.0 );
                        kernels[285] = compute_kernel(x,   19.9  , 19.8  , 11.7  , 61.45  , 1018.4  , 8.0 );
                        kernels[286] = compute_kernel(x,   20.0  , 19.9  , 8.4  , 49.74  , 1023.5  , 8.0 );
                        kernels[287] = compute_kernel(x,   19.5  , 19.5  , 9.7  , 55.56  , 1016.4  , 8.0 );
                        kernels[288] = compute_kernel(x,   20.1  , 20.1  , 13.5  , 67.43  , 1012.4  , 6.0 );
                        kernels[289] = compute_kernel(x,   17.5  , 17.4  , 8.3  , 57.3  , 1020.0  , 8.0 );
                        kernels[290] = compute_kernel(x,   25.9  , 25.4  , 12.0  , 46.05  , 1011.7  , 8.0 );
                        kernels[291] = compute_kernel(x,   18.7  , 18.7  , 9.6  , 57.81  , 1016.4  , 7.0 );
                        kernels[292] = compute_kernel(x,   18.4  , 18.3  , 7.8  , 52.77  , 1015.8  , 6.0 );
                        kernels[293] = compute_kernel(x,   17.8  , 17.8  , 8.6  , 57.67  , 1016.1  , 6.0 );
                        kernels[294] = compute_kernel(x,   20.3  , 20.3  , 11.7  , 61.02  , 1012.9  , 8.0 );
                        kernels[295] = compute_kernel(x,   22.8  , 22.7  , 15.6  , 66.69  , 1016.7  , 8.0 );
                        kernels[296] = compute_kernel(x,   26.1  , 25.3  , 11.7  , 47.22  , 1017.8  , 8.0 );
                        kernels[297] = compute_kernel(x,   25.7  , 25.7  , 15.6  , 55.19  , 1018.8  , 7.0 );
                        kernels[298] = compute_kernel(x,   24.2  , 24.1  , 15.3  , 61.43  , 1017.8  , 8.0 );
                        kernels[299] = compute_kernel(x,   26.7  , 26.7  , 16.1  , 55.87  , 1013.9  , 7.0 );
                        kernels[300] = compute_kernel(x,   28.2  , 28.1  , 16.0  , 50.94  , 1014.0  , 8.0 );
                        kernels[301] = compute_kernel(x,   20.3  , 20.3  , 13.3  , 64.94  , 1003.2  , 6.0 );
                        kernels[302] = compute_kernel(x,   18.0  , 18.0  , 11.6  , 67.27  , 1016.2  , 5.0 );
                        kernels[303] = compute_kernel(x,   18.2  , 18.2  , 10.6  , 62.04  , 1011.5  , 5.0 );
                        kernels[304] = compute_kernel(x,   13.6  , 13.6  , 6.4  , 64.41  , 1021.9  , 6.0 );
                        kernels[305] = compute_kernel(x,   16.1  , 16.1  , 10.9  , 72.64  , 1020.8  , 5.0 );
                        kernels[306] = compute_kernel(x,   15.1  , 15.1  , 9.4  , 70.35  , 1020.9  , 6.0 );
                        kernels[307] = compute_kernel(x,   15.9  , 15.9  , 11.6  , 77.43  , 1023.1  , 4.0 );
                        kernels[308] = compute_kernel(x,   19.7  , 19.7  , 15.8  , 79.04  , 1024.2  , 5.0 );
                        kernels[309] = compute_kernel(x,   17.8  , 17.7  , 13.4  , 76.35  , 1023.2  , 4.0 );
                        kernels[310] = compute_kernel(x,   14.8  , 14.7  , 6.9  , 61.02  , 1023.4  , 6.0 );
                        kernels[311] = compute_kernel(x,   16.9  , 16.9  , 9.7  , 64.41  , 1019.2  , 6.0 );
                        kernels[312] = compute_kernel(x,   21.1  , 21.1  , 14.0  , 65.41  , 1023.3  , 6.0 );
                        kernels[313] = compute_kernel(x,   16.9  , 16.8  , 9.0  , 62.1  , 1028.2  , 6.0 );
                        kernels[314] = compute_kernel(x,   17.8  , 17.7  , 11.8  , 70.6  , 1017.6  , 6.0 );
                        kernels[315] = compute_kernel(x,   18.0  , 18.0  , 11.9  , 70.42  , 1010.0  , 6.0 );
                        kernels[316] = compute_kernel(x,   10.2  , 8.9  , 3.9  , 65.32  , 1011.3  , 5.0 );
                        kernels[317] = compute_kernel(x,   13.0  , 12.5  , 8.0  , 72.65  , 1013.6  , 6.0 );
                        kernels[318] = compute_kernel(x,   12.3  , 11.7  , 8.0  , 76.47  , 1012.2  , 6.0 );
                        kernels[319] = compute_kernel(x,   9.3  , 9.3  , 6.0  , 80.27  , 1022.5  , 2.0 );
                        kernels[320] = compute_kernel(x,   10.7  , 10.7  , 6.4  , 75.9  , 1025.1  , 4.0 );
                        kernels[321] = compute_kernel(x,   10.1  , 9.4  , 5.9  , 76.59  , 1024.8  , 5.0 );
                        kernels[322] = compute_kernel(x,   11.8  , 11.5  , 6.9  , 73.46  , 1015.6  , 4.0 );
                        kernels[323] = compute_kernel(x,   13.3  , 13.3  , 10.0  , 80.93  , 1007.7  , 4.0 );
                        kernels[324] = compute_kernel(x,   12.4  , 12.2  , 9.9  , 85.43  , 1008.4  , 3.0 );
                        kernels[325] = compute_kernel(x,   6.2  , 5.2  , 5.0  , 91.9  , 1027.0  , 1.0 );
                        kernels[326] = compute_kernel(x,   10.2  , 9.5  , 7.7  , 84.93  , 1017.1  , 2.0 );
                        kernels[327] = compute_kernel(x,   5.9  , 5.5  , 4.1  , 88.64  , 1021.1  , 2.0 );
                        kernels[328] = compute_kernel(x,   7.0  , 4.9  , 5.5  , 91.18  , 1024.2  , 1.0 );
                        kernels[329] = compute_kernel(x,   5.0  , 2.9  , 1.5  , 78.08  , 1025.5  , 2.0 );
                        kernels[330] = compute_kernel(x,   5.0  , 1.9  , 1.9  , 80.27  , 991.1  , 1.0 );
                        kernels[331] = compute_kernel(x,   1.0  , 0.0  , 0.7  , 97.97  , 999.3  , 0.0 );
                        kernels[332] = compute_kernel(x,   7.1  , 4.9  , 5.0  , 86.69  , 999.2  , 1.0 );
                        kernels[333] = compute_kernel(x,   1.8  , -1.5  , -2.1  , 75.77  , 1031.0  , 2.0 );
                        kernels[334] = compute_kernel(x,   6.4  , 3.9  , 4.3  , 86.62  , 1016.1  , 1.0 );
                        kernels[335] = compute_kernel(x,   2.6  , 0.1  , 0.5  , 85.93  , 990.1  , 1.0 );
                        kernels[336] = compute_kernel(x,   1.6  , -0.9  , 0.0  , 89.41  , 1002.4  , 1.0 );
                        kernels[337] = compute_kernel(x,   -0.4  , -2.4  , -1.3  , 93.4  , 1006.0  , 1.0 );
                        kernels[338] = compute_kernel(x,   3.6  , 0.8  , 1.4  , 86.24  , 1017.6  , 1.0 );
                        kernels[339] = compute_kernel(x,   0.1  , -3.3  , -1.2  , 91.4  , 1018.6  , 1.0 );
                        kernels[340] = compute_kernel(x,   0.1  , -1.8  , -0.7  , 94.33  , 1026.4  , 1.0 );
                        kernels[341] = compute_kernel(x,   5.4  , 2.7  , 1.8  , 78.0  , 1023.6  , 2.0 );
                        kernels[342] = compute_kernel(x,   -1.2  , -6.7  , -4.6  , 77.87  , 1003.9  , 1.0 );
                        kernels[343] = compute_kernel(x,   -0.6  , -5.3  , -3.6  , 79.86  , 1007.0  , 1.0 );
                        kernels[344] = compute_kernel(x,   -0.1  , -3.7  , -5.0  , 70.12  , 1012.3  , 4.0 );
                        kernels[345] = compute_kernel(x,   -1.3  , -5.1  , -5.6  , 73.19  , 1025.1  , 3.0 );
                        kernels[346] = compute_kernel(x,   -0.7  , -5.9  , -7.3  , 61.1  , 1025.1  , 4.0 );
                        kernels[347] = compute_kernel(x,   8.3  , 5.5  , 5.5  , 83.14  , 1009.0  , 1.0 );
                        kernels[348] = compute_kernel(x,   12.6  , 12.6  , 6.8  , 67.78  , 1002.0  , 4.0 );
                        kernels[349] = compute_kernel(x,   10.0  , 8.9  , 5.6  , 76.12  , 1022.9  , 4.0 );
                        kernels[350] = compute_kernel(x,   10.2  , 9.4  , 5.2  , 72.36  , 1026.8  , 2.0 );
                        kernels[351] = compute_kernel(x,   5.7  , 3.7  , 3.0  , 83.47  , 1037.2  , 4.0 );
                        kernels[352] = compute_kernel(x,   6.1  , 3.8  , 3.1  , 81.81  , 1032.0  , 5.0 );
                        kernels[353] = compute_kernel(x,   5.3  , 3.3  , 2.8  , 84.21  , 1030.0  , 4.0 );
                        kernels[354] = compute_kernel(x,   7.0  , 5.0  , 1.2  , 68.16  , 1020.1  , 6.0 );
                        kernels[355] = compute_kernel(x,   10.0  , 8.8  , 5.6  , 74.44  , 1027.6  , 3.0 );
                        kernels[356] = compute_kernel(x,   7.5  , 5.9  , 4.3  , 80.94  , 1028.9  , 2.0 );
                        kernels[357] = compute_kernel(x,   7.4  , 6.6  , 4.0  , 79.48  , 1031.2  , 2.0 );
                        kernels[358] = compute_kernel(x,   7.6  , 6.8  , 3.2  , 75.27  , 1024.2  , 5.0 );
                        kernels[359] = compute_kernel(x,   8.4  , 7.6  , 5.2  , 81.16  , 1020.9  , 4.0 );
                        kernels[360] = compute_kernel(x,   8.4  , 7.0  , 5.4  , 82.35  , 1018.4  , 3.0 );
                        kernels[361] = compute_kernel(x,   12.7  , 12.2  , 6.4  , 68.1  , 1026.6  , 7.0 );
                        kernels[362] = compute_kernel(x,   14.1  , 14.1  , 6.6  , 61.72  , 1020.5  , 3.0 );
                        kernels[363] = compute_kernel(x,   10.8  , 9.6  , 5.8  , 71.87  , 1024.3  , 5.0 );
                        kernels[364] = compute_kernel(x,   8.4  , 7.7  , 1.7  , 65.1  , 1026.1  , 7.0 );
                        kernels[365] = compute_kernel(x,   6.2  , 3.1  , -4.2  , 51.84  , 1018.4  , 7.0 );
                        kernels[366] = compute_kernel(x,   3.1  , -0.4  , -5.0  , 57.54  , 1020.8  , 8.0 );
                        kernels[367] = compute_kernel(x,   8.7  , 7.3  , 2.8  , 67.93  , 1013.6  , 6.0 );
                        kernels[368] = compute_kernel(x,   5.8  , 5.6  , -0.4  , 66.5  , 1032.6  , 6.0 );
                        kernels[369] = compute_kernel(x,   7.3  , 6.7  , -0.5  , 61.86  , 1032.7  , 6.0 );
                        kernels[370] = compute_kernel(x,   6.1  , 4.7  , -0.4  , 64.52  , 1033.2  , 7.0 );
                        kernels[371] = compute_kernel(x,   6.2  , 5.3  , -1.1  , 63.33  , 1031.3  , 8.0 );
                        kernels[372] = compute_kernel(x,   8.8  , 8.4  , -3.0  , 47.71  , 1025.1  , 8.0 );
                        kernels[373] = compute_kernel(x,   10.2  , 9.5  , 4.5  , 68.83  , 1021.0  , 8.0 );
                        kernels[374] = compute_kernel(x,   9.0  , 7.4  , 1.6  , 61.26  , 1028.1  , 8.0 );
                        kernels[375] = compute_kernel(x,   8.1  , 6.1  , -0.5  , 56.34  , 1021.7  , 8.0 );
                        kernels[376] = compute_kernel(x,   9.4  , 9.2  , 0.5  , 58.04  , 1010.5  , 7.0 );
                        kernels[377] = compute_kernel(x,   8.0  , 7.1  , 0.6  , 61.7  , 1015.1  , 5.0 );
                        kernels[378] = compute_kernel(x,   8.1  , 7.4  , 0.8  , 62.64  , 1016.4  , 7.0 );
                        kernels[379] = compute_kernel(x,   9.1  , 8.8  , 1.3  , 61.22  , 1020.3  , 6.0 );
                        kernels[380] = compute_kernel(x,   9.5  , 8.4  , -0.8  , 50.31  , 1015.2  , 7.0 );
                        kernels[381] = compute_kernel(x,   15.9  , 15.9  , 10.6  , 71.0  , 997.3  , 6.0 );
                        kernels[382] = compute_kernel(x,   11.0  , 10.5  , 7.1  , 77.35  , 1013.3  , 3.0 );
                        kernels[383] = compute_kernel(x,   11.5  , 10.9  , 5.9  , 70.7  , 1014.7  , 6.0 );
                        kernels[384] = compute_kernel(x,   14.9  , 14.9  , 7.3  , 63.22  , 1020.0  , 10.0 );
                        kernels[385] = compute_kernel(x,   15.8  , 15.8  , 8.2  , 62.24  , 1026.5  , 9.0 );
                        kernels[386] = compute_kernel(x,   13.3  , 13.0  , 8.2  , 73.14  , 1027.6  , 9.0 );
                        kernels[387] = compute_kernel(x,   15.7  , 15.4  , 8.7  , 67.22  , 1021.6  , 8.0 );
                        kernels[388] = compute_kernel(x,   20.4  , 20.4  , 10.3  , 54.17  , 1012.6  , 8.0 );
                        kernels[389] = compute_kernel(x,   17.3  , 17.3  , 10.2  , 67.73  , 1024.3  , 9.0 );
                        kernels[390] = compute_kernel(x,   17.9  , 17.9  , 12.8  , 73.56  , 1023.9  , 9.0 );
                        kernels[391] = compute_kernel(x,   19.4  , 19.4  , 14.6  , 74.93  , 1020.9  , 8.0 );
                        kernels[392] = compute_kernel(x,   19.4  , 19.4  , 11.1  , 61.49  , 1023.3  , 9.0 );
                        kernels[393] = compute_kernel(x,   22.0  , 21.9  , 13.1  , 58.4  , 1020.9  , 8.0 );
                        kernels[394] = compute_kernel(x,   20.4  , 20.4  , 10.3  , 53.37  , 1020.6  , 7.0 );
                        kernels[395] = compute_kernel(x,   21.8  , 21.8  , 14.3  , 64.93  , 1012.3  , 8.0 );
                        kernels[396] = compute_kernel(x,   14.0  , 14.0  , 7.0  , 64.23  , 1020.3  , 6.0 );
                        kernels[397] = compute_kernel(x,   15.5  , 15.3  , 6.7  , 59.13  , 1023.4  , 8.0 );
                        kernels[398] = compute_kernel(x,   16.1  , 16.1  , 11.4  , 74.52  , 1021.6  , 6.0 );
                        kernels[399] = compute_kernel(x,   17.4  , 17.4  , 14.4  , 82.76  , 1015.6  , 2.0 );
                        kernels[400] = compute_kernel(x,   15.2  , 15.2  , 10.7  , 74.85  , 1017.4  , 4.0 );
                        kernels[401] = compute_kernel(x,   17.5  , 17.5  , 11.7  , 69.88  , 1017.8  , 6.0 );
                        kernels[402] = compute_kernel(x,   19.0  , 19.0  , 13.7  , 73.35  , 1016.0  , 8.0 );
                        kernels[403] = compute_kernel(x,   16.8  , 16.7  , 12.6  , 77.19  , 1011.2  , 5.0 );
                        kernels[404] = compute_kernel(x,   18.8  , 18.8  , 13.5  , 72.99  , 1020.6  , 7.0 );
                        kernels[405] = compute_kernel(x,   16.4  , 16.4  , 12.7  , 80.08  , 1015.3  , 6.0 );
                        kernels[406] = compute_kernel(x,   19.8  , 19.8  , 13.4  , 67.8  , 1017.9  , 8.0 );
                        kernels[407] = compute_kernel(x,   17.3  , 17.3  , 10.1  , 64.79  , 1009.6  , 7.0 );
                        kernels[408] = compute_kernel(x,   16.1  , 16.1  , 11.1  , 73.49  , 1014.9  , 4.0 );
                        kernels[409] = compute_kernel(x,   17.8  , 17.8  , 11.6  , 69.95  , 1012.8  , 7.0 );
                        kernels[410] = compute_kernel(x,   18.3  , 18.3  , 11.9  , 68.32  , 1018.0  , 8.0 );
                        kernels[411] = compute_kernel(x,   17.7  , 17.6  , 13.6  , 77.99  , 1020.8  , 5.0 );
                        kernels[412] = compute_kernel(x,   19.1  , 19.0  , 13.8  , 73.22  , 1019.9  , 7.0 );
                        kernels[413] = compute_kernel(x,   18.6  , 18.5  , 13.5  , 73.81  , 1013.6  , 8.0 );
                        kernels[414] = compute_kernel(x,   16.4  , 16.3  , 10.3  , 68.35  , 1016.9  , 4.0 );
                        kernels[415] = compute_kernel(x,   15.8  , 15.8  , 11.9  , 77.83  , 1019.8  , 2.0 );
                        kernels[416] = compute_kernel(x,   17.2  , 17.2  , 12.5  , 74.32  , 1014.3  , 7.0 );
                        kernels[417] = compute_kernel(x,   17.9  , 17.8  , 12.5  , 72.25  , 1027.5  , 7.0 );
                        kernels[418] = compute_kernel(x,   17.4  , 17.4  , 11.2  , 69.22  , 1029.4  , 7.0 );
                        kernels[419] = compute_kernel(x,   17.2  , 17.2  , 11.7  , 71.53  , 1025.8  , 8.0 );
                        kernels[420] = compute_kernel(x,   15.8  , 15.8  , 9.8  , 68.39  , 1023.4  , 3.0 );
                        kernels[421] = compute_kernel(x,   15.8  , 15.8  , 11.1  , 74.57  , 1025.9  , 6.0 );
                        kernels[422] = compute_kernel(x,   17.5  , 17.5  , 12.1  , 71.23  , 1021.8  , 3.0 );
                        kernels[423] = compute_kernel(x,   16.7  , 16.6  , 12.6  , 77.11  , 1018.9  , 3.0 );
                        kernels[424] = compute_kernel(x,   19.0  , 19.0  , 13.6  , 72.92  , 1020.4  , 6.0 );
                        kernels[425] = compute_kernel(x,   18.8  , 18.7  , 13.4  , 72.58  , 1016.0  , 6.0 );
                        kernels[426] = compute_kernel(x,   16.8  , 16.8  , 11.2  , 70.52  , 1018.7  , 5.0 );
                        kernels[427] = compute_kernel(x,   17.1  , 17.1  , 12.4  , 74.69  , 1017.8  , 5.0 );
                        kernels[428] = compute_kernel(x,   17.4  , 17.4  , 13.1  , 76.5  , 1014.1  , 3.0 );
                        kernels[429] = compute_kernel(x,   16.6  , 16.6  , 12.7  , 79.48  , 1014.7  , 6.0 );
                        kernels[430] = compute_kernel(x,   15.8  , 15.7  , 11.6  , 76.9  , 1022.7  , 4.0 );
                        kernels[431] = compute_kernel(x,   16.0  , 16.0  , 12.8  , 82.0  , 1020.5  , 3.0 );
                        kernels[432] = compute_kernel(x,   17.1  , 17.1  , 13.1  , 78.96  , 1016.2  , 6.0 );
                        kernels[433] = compute_kernel(x,   17.0  , 16.9  , 15.2  , 89.35  , 1014.3  , 1.0 );
                        kernels[434] = compute_kernel(x,   16.7  , 16.7  , 13.6  , 83.4  , 1011.7  , 4.0 );
                        kernels[435] = compute_kernel(x,   14.9  , 14.9  , 13.5  , 91.79  , 1025.9  , 2.0 );
                        kernels[436] = compute_kernel(x,   13.3  , 13.3  , 10.0  , 81.34  , 1029.5  , 5.0 );
                        kernels[437] = compute_kernel(x,   11.8  , 11.3  , 9.2  , 85.05  , 1026.1  , 5.0 );
                        kernels[438] = compute_kernel(x,   12.2  , 12.2  , 9.4  , 83.08  , 1027.8  , 3.0 );
                        kernels[439] = compute_kernel(x,   14.0  , 13.9  , 10.3  , 80.03  , 1023.9  , 5.0 );
                        kernels[440] = compute_kernel(x,   13.0  , 13.0  , 8.7  , 75.96  , 1020.3  , 3.0 );
                        kernels[441] = compute_kernel(x,   9.5  , 8.2  , 5.3  , 75.44  , 1020.9  , 5.0 );
                        kernels[442] = compute_kernel(x,   11.4  , 11.3  , 7.1  , 75.53  , 1024.9  , 2.0 );
                        kernels[443] = compute_kernel(x,   13.9  , 13.9  , 10.7  , 81.11  , 1009.0  , 4.0 );
                        kernels[444] = compute_kernel(x,   7.4  , 5.3  , 3.7  , 77.7  , 1015.0  , 2.0 );
                        kernels[445] = compute_kernel(x,   5.9  , 4.2  , 3.0  , 82.84  , 1026.4  , 4.0 );
                        kernels[446] = compute_kernel(x,   11.2  , 10.6  , 7.8  , 80.08  , 1024.7  , 2.0 );
                        kernels[447] = compute_kernel(x,   8.3  , 7.6  , 6.0  , 85.61  , 1023.7  , 2.0 );
                        kernels[448] = compute_kernel(x,   12.9  , 12.8  , 9.6  , 81.0  , 1023.3  , 3.0 );
                        kernels[449] = compute_kernel(x,   11.3  , 11.2  , 9.6  , 89.63  , 1022.6  , 1.0 );
                        kernels[450] = compute_kernel(x,   11.5  , 11.5  , 8.8  , 83.65  , 1024.0  , 1.0 );
                        kernels[451] = compute_kernel(x,   7.6  , 7.4  , 5.5  , 86.76  , 1022.9  , 3.0 );
                        kernels[452] = compute_kernel(x,   9.7  , 9.1  , 7.5  , 86.07  , 1028.9  , 1.0 );
                        kernels[453] = compute_kernel(x,   6.7  , 4.2  , 2.8  , 76.64  , 1019.8  , 2.0 );
                        kernels[454] = compute_kernel(x,   5.0  , 2.9  , 1.1  , 76.55  , 1016.4  , 3.0 );
                        kernels[455] = compute_kernel(x,   1.0  , -2.1  , -0.9  , 86.99  , 1014.5  , 1.0 );
                        kernels[456] = compute_kernel(x,   10.0  , 8.8  , 7.2  , 82.71  , 1010.2  , 1.0 );
                        kernels[457] = compute_kernel(x,   6.6  , 2.7  , 1.3  , 69.25  , 990.9  , 2.0 );
                        kernels[458] = compute_kernel(x,   6.5  , 3.5  , 2.5  , 76.14  , 999.1  , 3.0 );
                        kernels[459] = compute_kernel(x,   4.5  , 2.8  , 0.8  , 77.47  , 1025.3  , 1.0 );
                        kernels[460] = compute_kernel(x,   5.4  , 4.0  , 3.9  , 90.37  , 1029.3  , 0.0 );
                        kernels[461] = compute_kernel(x,   8.8  , 7.2  , 7.1  , 89.27  , 1020.4  , 1.0 );
                        kernels[462] = compute_kernel(x,   7.8  , 5.1  , 5.0  , 82.9  , 1033.2  , 1.0 );
                        kernels[463] = compute_kernel(x,   7.4  , 6.5  , 5.3  , 86.62  , 1031.5  , 1.0 );
                        kernels[464] = compute_kernel(x,   6.4  , 5.5  , 5.2  , 91.74  , 1023.3  , 1.0 );
                        kernels[465] = compute_kernel(x,   6.0  , 3.8  , 4.5  , 89.94  , 1018.5  , 1.0 );
                        kernels[466] = compute_kernel(x,   11.7  , 11.6  , 8.7  , 82.64  , 1024.7  , 3.0 );
                        kernels[467] = compute_kernel(x,   12.1  , 12.1  , 6.0  , 66.12  , 1012.7  , 3.0 );
                        kernels[468] = compute_kernel(x,   10.4  , 10.0  , 7.3  , 81.2  , 1022.8  , 2.0 );
                        kernels[469] = compute_kernel(x,   9.7  , 9.4  , 6.9  , 83.5  , 1023.8  , 2.0 );
                        kernels[470] = compute_kernel(x,   15.1  , 15.1  , 12.9  , 86.6  , 1016.1  , 2.0 );
                        kernels[471] = compute_kernel(x,   11.0  , 11.0  , 7.7  , 79.84  , 1026.7  , 1.0 );
                        kernels[472] = compute_kernel(x,   8.7  , 7.4  , 7.5  , 92.03  , 1018.5  , 1.0 );
                        kernels[473] = compute_kernel(x,   7.8  , 6.8  , 6.6  , 91.72  , 1025.9  , 1.0 );
                        kernels[474] = compute_kernel(x,   6.8  , 3.9  , 4.9  , 87.42  , 999.1  , 1.0 );
                        kernels[475] = compute_kernel(x,   0.3  , -1.5  , -0.9  , 92.16  , 1018.6  , 1.0 );
                        kernels[476] = compute_kernel(x,   0.9  , -2.5  , 0.1  , 94.97  , 1026.1  , 1.0 );
                        kernels[477] = compute_kernel(x,   6.7  , 3.1  , 4.3  , 85.12  , 1017.8  , 1.0 );
                        kernels[478] = compute_kernel(x,   10.7  , 9.5  , 6.2  , 73.97  , 1022.8  , 3.0 );
                        kernels[479] = compute_kernel(x,   7.1  , 4.4  , 0.7  , 64.51  , 1032.2  , 3.0 );
                        kernels[480] = compute_kernel(x,   6.8  , 4.5  , 1.2  , 68.22  , 1013.2  , 2.0 );
                        kernels[481] = compute_kernel(x,   17.1  , 17.1  , 10.4  , 65.58  , 1022.2  , 3.0 );
                        kernels[482] = compute_kernel(x,   15.6  , 15.6  , 11.3  , 75.57  , 1026.1  , 3.0 );
                        kernels[483] = compute_kernel(x,   15.7  , 15.6  , 11.6  , 76.93  , 1029.3  , 3.0 );
                        kernels[484] = compute_kernel(x,   16.1  , 16.0  , 11.2  , 73.44  , 1030.8  , 3.0 );
                        kernels[485] = compute_kernel(x,   17.0  , 17.0  , 11.1  , 68.98  , 1027.8  , 4.0 );
                        kernels[486] = compute_kernel(x,   15.4  , 15.3  , 11.5  , 77.78  , 1018.0  , 5.0 );
                        kernels[487] = compute_kernel(x,   10.2  , 10.1  , 7.4  , 83.18  , 1027.0  , 1.0 );
                        kernels[488] = compute_kernel(x,   11.3  , 11.3  , 9.2  , 86.88  , 1030.6  , 1.0 );
                        kernels[489] = compute_kernel(x,   10.1  , 9.9  , 7.7  , 84.81  , 1024.1  , 0.0 );
                        kernels[490] = compute_kernel(x,   5.3  , 4.4  , 3.4  , 88.11  , 1022.1  , 1.0 );
                        kernels[491] = compute_kernel(x,   10.3  , 10.0  , 8.7  , 89.71  , 1023.2  , 1.0 );
                        kernels[492] = compute_kernel(x,   11.0  , 10.9  , 9.1  , 88.15  , 1026.6  , 1.0 );
                        kernels[493] = compute_kernel(x,   11.9  , 11.9  , 8.4  , 79.2  , 1030.6  , 0.0 );
                        kernels[494] = compute_kernel(x,   11.2  , 11.2  , 6.3  , 71.96  , 1038.0  , 0.0 );
                        kernels[495] = compute_kernel(x,   9.0  , 7.5  , 6.1  , 82.7  , 1040.5  , 1.0 );
                        kernels[496] = compute_kernel(x,   8.3  , 6.2  , 7.0  , 91.88  , 1039.3  , 1.0 );
                        kernels[497] = compute_kernel(x,   5.0  , 3.9  , 4.5  , 96.08  , 1034.8  , 1.0 );
                        kernels[498] = compute_kernel(x,   6.3  , 4.5  , 3.1  , 80.7  , 1028.9  , 1.0 );
                        kernels[499] = compute_kernel(x,   14.0  , 14.0  , 12.0  , 87.59  , 1012.3  , 1.0 );
                        kernels[500] = compute_kernel(x,   -0.3  , -3.3  , -1.4  , 92.47  , 997.1  , 0.0 );
                        decisions[0] = 4.279677825228
                        + kernels[0] * 120.0
                        + kernels[1] * 120.0
                        + kernels[2] * 120.0
                        + kernels[3] * 120.0
                        + kernels[4] * 120.0
                        + kernels[9] * 120.0
                        + kernels[10] * 120.0
                        + kernels[15] * 120.0
                        + kernels[19] * 65.587290582926
                        + kernels[20] * 120.0
                        + kernels[22] * 120.0
                        + kernels[40] * 120.0
                        + kernels[41] * 120.0
                        + kernels[42] * 120.0
                        + kernels[43] * 120.0
                        + kernels[44] * 120.0
                        + kernels[45] * 120.0
                        + kernels[46] * 120.0
                        + kernels[47] * 120.0
                        + kernels[48] * 120.0
                        + kernels[51] * 120.0
                        + kernels[52] * 120.0
                        + kernels[57] * 74.50889518406
                        + kernels[64] * 120.0
                        + kernels[67] * 120.0
                        + kernels[68] * 120.0
                        + kernels[69] * 120.0
                        + kernels[70] * 120.0
                        + kernels[71] * 120.0
                        + kernels[72] * 120.0
                        + kernels[73] * 120.0
                        + kernels[74] * 120.0
                        + kernels[75] * 120.0
                        + kernels[76] * 120.0
                        + kernels[77] * 120.0
                        + kernels[78] * 120.0
                        + kernels[79] * 120.0
                        + kernels[80] * 120.0
                        + kernels[81] * 120.0
                        + kernels[87] * -111.072411585571
                        + kernels[88] * -89.483134416977
                        + kernels[103] * -66.65988985238
                        + kernels[105] * -120.0
                        + kernels[106] * -120.0
                        + kernels[107] * -120.0
                        + kernels[111] * -120.0
                        + kernels[117] * -100.965577840794
                        + kernels[118] * -120.0
                        + kernels[121] * -120.0
                        + kernels[122] * -120.0
                        + kernels[124] * -120.0
                        + kernels[127] * -120.0
                        + kernels[128] * -120.0
                        + kernels[129] * -120.0
                        + kernels[130] * -120.0
                        + kernels[131] * -120.0
                        + kernels[135] * -120.0
                        + kernels[137] * -120.0
                        + kernels[138] * -44.153022926658
                        + kernels[140] * -120.0
                        + kernels[141] * -120.0
                        + kernels[148] * -120.0
                        + kernels[156] * -39.223397179401
                        + kernels[162] * -32.482285411759
                        + kernels[164] * -16.056466553445
                        + kernels[180] * -120.0
                        + kernels[181] * -120.0
                        + kernels[185] * -120.0
                        + kernels[186] * -120.0
                        + kernels[187] * -120.0
                        + kernels[189] * -120.0
                        + kernels[190] * -120.0
                        + kernels[192] * -120.0
                        + kernels[198] * -120.0
                        + kernels[200] * -120.0
                        + kernels[201] * -120.0
                        + kernels[203] * -120.0
                        + kernels[204] * -120.0
                        + kernels[220] * -120.0
                        + kernels[223] * -120.0
                        + kernels[224] * -120.0
                        ;
                        decisions[1] = 1.101615874224
                        + kernels[0] * 120.0
                        + kernels[1] * 120.0
                        + kernels[2] * 120.0
                        + kernels[3] * 120.0
                        + kernels[4] * 120.0
                        + kernels[5] * 120.0
                        + kernels[6] * 120.0
                        + kernels[7] * 120.0
                        + kernels[8] * 120.0
                        + kernels[9] * 120.0
                        + kernels[10] * 120.0
                        + kernels[11] * 120.0
                        + kernels[12] * 120.0
                        + kernels[13] * 120.0
                        + kernels[14] * 120.0
                        + kernels[15] * 120.0
                        + kernels[16] * 120.0
                        + kernels[17] * 120.0
                        + kernels[18] * 120.0
                        + kernels[19] * 120.0
                        + kernels[20] * 120.0
                        + kernels[21] * 120.0
                        + kernels[22] * 120.0
                        + kernels[23] * 120.0
                        + kernels[24] * 120.0
                        + kernels[25] * 120.0
                        + kernels[26] * 120.0
                        + kernels[27] * 120.0
                        + kernels[28] * 120.0
                        + kernels[29] * 120.0
                        + kernels[30] * 120.0
                        + kernels[31] * 120.0
                        + kernels[32] * 120.0
                        + kernels[33] * 120.0
                        + kernels[34] * 120.0
                        + kernels[35] * 120.0
                        + kernels[36] * 120.0
                        + kernels[37] * 120.0
                        + kernels[38] * 120.0
                        + kernels[39] * 120.0
                        + kernels[40] * 120.0
                        + kernels[41] * 120.0
                        + kernels[42] * 120.0
                        + kernels[43] * 120.0
                        + kernels[44] * 120.0
                        + kernels[45] * 120.0
                        + kernels[46] * 120.0
                        + kernels[47] * 120.0
                        + kernels[48] * 120.0
                        + kernels[49] * 120.0
                        + kernels[50] * 120.0
                        + kernels[51] * 120.0
                        + kernels[52] * 120.0
                        + kernels[53] * 120.0
                        + kernels[54] * 120.0
                        + kernels[55] * 120.0
                        + kernels[56] * 120.0
                        + kernels[57] * 120.0
                        + kernels[58] * 120.0
                        + kernels[59] * 120.0
                        + kernels[60] * 120.0
                        + kernels[61] * 120.0
                        + kernels[62] * 120.0
                        + kernels[63] * 120.0
                        + kernels[64] * 120.0
                        + kernels[65] * 120.0
                        + kernels[66] * 120.0
                        + kernels[67] * 120.0
                        + kernels[68] * 120.0
                        + kernels[69] * 120.0
                        + kernels[70] * 120.0
                        + kernels[71] * 120.0
                        + kernels[72] * 120.0
                        + kernels[73] * 120.0
                        + kernels[74] * 120.0
                        + kernels[75] * 120.0
                        + kernels[76] * 120.0
                        + kernels[77] * 120.0
                        + kernels[78] * 120.0
                        + kernels[79] * 120.0
                        + kernels[80] * 120.0
                        + kernels[81] * 120.0
                        + kernels[238] * -51.888216070654
                        + kernels[241] * -120.0
                        + kernels[249] * -104.502305932088
                        + kernels[252] * -49.656481840871
                        + kernels[253] * -120.0
                        + kernels[254] * -120.0
                        + kernels[255] * -120.0
                        + kernels[256] * -120.0
                        + kernels[258] * -120.0
                        + kernels[262] * -120.0
                        + kernels[263] * -120.0
                        + kernels[265] * -120.0
                        + kernels[266] * -120.0
                        + kernels[267] * -120.0
                        + kernels[268] * -120.0
                        + kernels[269] * -120.0
                        + kernels[270] * -120.0
                        + kernels[271] * -120.0
                        + kernels[272] * -120.0
                        + kernels[273] * -120.0
                        + kernels[274] * -120.0
                        + kernels[275] * -120.0
                        + kernels[276] * -120.0
                        + kernels[278] * -79.08883972516
                        + kernels[279] * -120.0
                        + kernels[280] * -120.0
                        + kernels[281] * -120.0
                        + kernels[282] * -120.0
                        + kernels[284] * -120.0
                        + kernels[285] * -120.0
                        + kernels[286] * -120.0
                        + kernels[287] * -120.0
                        + kernels[289] * -120.0
                        + kernels[290] * -120.0
                        + kernels[291] * -120.0
                        + kernels[292] * -120.0
                        + kernels[293] * -120.0
                        + kernels[294] * -120.0
                        + kernels[295] * -50.657584532176
                        + kernels[296] * -120.0
                        + kernels[297] * -120.0
                        + kernels[298] * -120.0
                        + kernels[299] * -120.0
                        + kernels[300] * -120.0
                        + kernels[304] * -120.0
                        + kernels[308] * -36.076957586534
                        + kernels[310] * -120.0
                        + kernels[311] * -120.0
                        + kernels[312] * -120.0
                        + kernels[313] * -120.0
                        + kernels[325] * -120.0
                        + kernels[331] * -120.0
                        + kernels[337] * -28.841595841422
                        + kernels[340] * -101.272312345085
                        + kernels[361] * -120.0
                        + kernels[364] * -120.0
                        + kernels[365] * -120.0
                        + kernels[368] * -120.0
                        + kernels[369] * -120.0
                        + kernels[370] * -104.391433476952
                        + kernels[371] * -120.0
                        + kernels[372] * -120.0
                        + kernels[374] * -120.0
                        + kernels[375] * -120.0
                        + kernels[376] * -120.0
                        + kernels[379] * -120.0
                        + kernels[380] * -120.0
                        + kernels[384] * -120.0
                        + kernels[385] * -120.0
                        + kernels[386] * -120.0
                        + kernels[387] * -120.0
                        + kernels[388] * -120.0
                        + kernels[389] * -120.0
                        + kernels[390] * -120.0
                        + kernels[391] * -25.177029997188
                        + kernels[392] * -120.0
                        + kernels[393] * -120.0
                        + kernels[394] * -120.0
                        + kernels[396] * -120.0
                        + kernels[397] * -120.0
                        + kernels[417] * -120.0
                        + kernels[418] * -120.0
                        + kernels[419] * -120.0
                        + kernels[421] * -120.0
                        + kernels[435] * -120.0
                        + kernels[436] * -120.0
                        + kernels[437] * -88.44724265187
                        ;
                        decisions[2] = 3.638756905945
                        + kernels[0] * 120.0
                        + kernels[1] * 120.0
                        + kernels[2] * 120.0
                        + kernels[41] * 120.0
                        + kernels[43] * 120.0
                        + kernels[44] * 52.626553277508
                        + kernels[45] * 39.972324249977
                        + kernels[46] * 120.0
                        + kernels[47] * 120.0
                        + kernels[68] * 120.0
                        + kernels[69] * 23.615089347632
                        + kernels[73] * 120.0
                        + kernels[74] * 100.556554366493
                        + kernels[76] * 120.0
                        + kernels[77] * 120.0
                        + kernels[78] * 21.009444632826
                        + kernels[79] * 120.0
                        + kernels[80] * 120.0
                        + kernels[81] * 120.0
                        + kernels[466] * -120.0
                        + kernels[467] * -120.0
                        + kernels[468] * -3.739818446347
                        + kernels[469] * -120.0
                        + kernels[475] * -120.0
                        + kernels[476] * -120.0
                        + kernels[478] * -120.0
                        + kernels[479] * -120.0
                        + kernels[480] * -120.0
                        + kernels[481] * -120.0
                        + kernels[482] * -120.0
                        + kernels[483] * -120.0
                        + kernels[484] * -120.0
                        + kernels[485] * -120.0
                        + kernels[486] * -120.0
                        + kernels[495] * -54.468306804446
                        + kernels[496] * -59.571840623644
                        + kernels[497] * -120.0
                        ;
                        decisions[3] = 0.744220443252
                        + kernels[2] * 6.127233239061
                        + kernels[78] * 9.769058273144
                        + kernels[81] * 4.446477044094
                        + kernels[500] * -20.342768556299
                        ;
                        decisions[4] = -3.084944430167
                        + kernels[82] * 120.0
                        + kernels[83] * 120.0
                        + kernels[84] * 120.0
                        + kernels[85] * 120.0
                        + kernels[86] * 120.0
                        + kernels[87] * 120.0
                        + kernels[88] * 120.0
                        + kernels[89] * 120.0
                        + kernels[90] * 120.0
                        + kernels[91] * 120.0
                        + kernels[92] * 120.0
                        + kernels[93] * 120.0
                        + kernels[94] * 120.0
                        + kernels[95] * 120.0
                        + kernels[96] * 120.0
                        + kernels[97] * 120.0
                        + kernels[98] * 120.0
                        + kernels[99] * 120.0
                        + kernels[100] * 120.0
                        + kernels[101] * 120.0
                        + kernels[102] * 120.0
                        + kernels[103] * 120.0
                        + kernels[105] * 120.0
                        + kernels[106] * 120.0
                        + kernels[107] * 120.0
                        + kernels[108] * 120.0
                        + kernels[109] * 120.0
                        + kernels[110] * 120.0
                        + kernels[111] * 120.0
                        + kernels[112] * 120.0
                        + kernels[113] * 100.966870511511
                        + kernels[114] * 120.0
                        + kernels[115] * 120.0
                        + kernels[116] * 120.0
                        + kernels[117] * 120.0
                        + kernels[118] * 120.0
                        + kernels[119] * 120.0
                        + kernels[120] * 120.0
                        + kernels[121] * 120.0
                        + kernels[122] * 120.0
                        + kernels[123] * 120.0
                        + kernels[124] * 120.0
                        + kernels[125] * 120.0
                        + kernels[126] * 120.0
                        + kernels[127] * 120.0
                        + kernels[128] * 120.0
                        + kernels[129] * 120.0
                        + kernels[130] * 120.0
                        + kernels[131] * 120.0
                        + kernels[132] * 120.0
                        + kernels[133] * 120.0
                        + kernels[134] * 120.0
                        + kernels[135] * 120.0
                        + kernels[136] * 120.0
                        + kernels[137] * 120.0
                        + kernels[138] * 120.0
                        + kernels[139] * 120.0
                        + kernels[140] * 120.0
                        + kernels[141] * 120.0
                        + kernels[142] * 120.0
                        + kernels[143] * 120.0
                        + kernels[144] * 120.0
                        + kernels[145] * 120.0
                        + kernels[146] * 120.0
                        + kernels[147] * 120.0
                        + kernels[148] * 120.0
                        + kernels[149] * 120.0
                        + kernels[150] * 33.792922337945
                        + kernels[152] * 120.0
                        + kernels[153] * 120.0
                        + kernels[154] * 120.0
                        + kernels[155] * 120.0
                        + kernels[156] * 120.0
                        + kernels[157] * 120.0
                        + kernels[158] * 120.0
                        + kernels[159] * 120.0
                        + kernels[160] * 120.0
                        + kernels[161] * 79.9240684686
                        + kernels[163] * 120.0
                        + kernels[164] * 120.0
                        + kernels[166] * 120.0
                        + kernels[167] * 120.0
                        + kernels[168] * 120.0
                        + kernels[169] * 120.0
                        + kernels[172] * 120.0
                        + kernels[173] * 120.0
                        + kernels[175] * 120.0
                        + kernels[176] * 120.0
                        + kernels[177] * 120.0
                        + kernels[178] * 120.0
                        + kernels[179] * 120.0
                        + kernels[180] * 120.0
                        + kernels[181] * 120.0
                        + kernels[182] * 34.858263595039
                        + kernels[183] * 120.0
                        + kernels[184] * 120.0
                        + kernels[185] * 120.0
                        + kernels[186] * 120.0
                        + kernels[187] * 120.0
                        + kernels[188] * 120.0
                        + kernels[189] * 120.0
                        + kernels[190] * 120.0
                        + kernels[191] * 120.0
                        + kernels[192] * 120.0
                        + kernels[193] * 120.0
                        + kernels[194] * 69.744794469797
                        + kernels[195] * 120.0
                        + kernels[196] * 120.0
                        + kernels[197] * 120.0
                        + kernels[198] * 120.0
                        + kernels[199] * 120.0
                        + kernels[200] * 120.0
                        + kernels[201] * 120.0
                        + kernels[202] * 120.0
                        + kernels[203] * 120.0
                        + kernels[204] * 120.0
                        + kernels[205] * 120.0
                        + kernels[206] * 120.0
                        + kernels[207] * 120.0
                        + kernels[208] * 120.0
                        + kernels[209] * 120.0
                        + kernels[210] * 120.0
                        + kernels[211] * 120.0
                        + kernels[212] * 120.0
                        + kernels[213] * 120.0
                        + kernels[214] * 120.0
                        + kernels[215] * 120.0
                        + kernels[216] * 120.0
                        + kernels[217] * 120.0
                        + kernels[218] * 120.0
                        + kernels[219] * 120.0
                        + kernels[220] * 120.0
                        + kernels[221] * 120.0
                        + kernels[222] * 120.0
                        + kernels[223] * 120.0
                        + kernels[224] * 120.0
                        + kernels[225] * 120.0
                        + kernels[226] * 120.0
                        + kernels[227] * 120.0
                        + kernels[229] * 120.0
                        + kernels[230] * 120.0
                        + kernels[231] * 120.0
                        + kernels[232] * 120.0
                        + kernels[233] * 120.0
                        + kernels[234] * 120.0
                        + kernels[235] * 120.0
                        + kernels[236] * -120.0
                        + kernels[237] * -120.0
                        + kernels[239] * -120.0
                        + kernels[240] * -120.0
                        + kernels[241] * -120.0
                        + kernels[242] * -120.0
                        + kernels[243] * -120.0
                        + kernels[244] * -120.0
                        + kernels[245] * -120.0
                        + kernels[246] * -120.0
                        + kernels[247] * -120.0
                        + kernels[248] * -120.0
                        + kernels[250] * -120.0
                        + kernels[251] * -120.0
                        + kernels[252] * -120.0
                        + kernels[257] * -120.0
                        + kernels[259] * -120.0
                        + kernels[260] * -120.0
                        + kernels[261] * -120.0
                        + kernels[264] * -120.0
                        + kernels[277] * -120.0
                        + kernels[278] * -120.0
                        + kernels[283] * -120.0
                        + kernels[288] * -120.0
                        + kernels[294] * -120.0
                        + kernels[301] * -120.0
                        + kernels[302] * -120.0
                        + kernels[303] * -120.0
                        + kernels[305] * -120.0
                        + kernels[306] * -120.0
                        + kernels[307] * -120.0
                        + kernels[308] * -114.064852056209
                        + kernels[314] * -120.0
                        + kernels[315] * -120.0
                        + kernels[316] * -120.0
                        + kernels[317] * -120.0
                        + kernels[318] * -120.0
                        + kernels[319] * -120.0
                        + kernels[320] * -120.0
                        + kernels[321] * -120.0
                        + kernels[322] * -120.0
                        + kernels[323] * -120.0
                        + kernels[324] * -120.0
                        + kernels[325] * -120.0
                        + kernels[326] * -120.0
                        + kernels[327] * -120.0
                        + kernels[328] * -120.0
                        + kernels[329] * -120.0
                        + kernels[330] * -120.0
                        + kernels[331] * -120.0
                        + kernels[332] * -120.0
                        + kernels[333] * -120.0
                        + kernels[334] * -120.0
                        + kernels[335] * -120.0
                        + kernels[336] * -120.0
                        + kernels[337] * -120.0
                        + kernels[338] * -120.0
                        + kernels[339] * -120.0
                        + kernels[340] * -120.0
                        + kernels[341] * -120.0
                        + kernels[342] * -120.0
                        + kernels[343] * -120.0
                        + kernels[344] * -120.0
                        + kernels[345] * -120.0
                        + kernels[346] * -59.435492057253
                        + kernels[347] * -120.0
                        + kernels[348] * -120.0
                        + kernels[349] * -120.0
                        + kernels[351] * -120.0
                        + kernels[352] * -120.0
                        + kernels[353] * -120.0
                        + kernels[354] * -120.0
                        + kernels[356] * -120.0
                        + kernels[357] * -120.0
                        + kernels[358] * -120.0
                        + kernels[359] * -120.0
                        + kernels[360] * -120.0
                        + kernels[363] * -120.0
                        + kernels[365] * -38.230214610666
                        + kernels[366] * -120.0
                        + kernels[367] * -120.0
                        + kernels[373] * -120.0
                        + kernels[376] * -120.0
                        + kernels[377] * -120.0
                        + kernels[378] * -120.0
                        + kernels[381] * -120.0
                        + kernels[382] * -120.0
                        + kernels[383] * -120.0
                        + kernels[391] * -120.0
                        + kernels[395] * -120.0
                        + kernels[398] * -120.0
                        + kernels[399] * -120.0
                        + kernels[400] * -120.0
                        + kernels[401] * -120.0
                        + kernels[402] * -120.0
                        + kernels[403] * -120.0
                        + kernels[404] * -120.0
                        + kernels[405] * -120.0
                        + kernels[406] * -25.802223241891
                        + kernels[407] * -120.0
                        + kernels[408] * -120.0
                        + kernels[409] * -120.0
                        + kernels[410] * -120.0
                        + kernels[411] * -120.0
                        + kernels[412] * -120.0
                        + kernels[413] * -120.0
                        + kernels[414] * -120.0
                        + kernels[415] * -120.0
                        + kernels[416] * -120.0
                        + kernels[423] * -120.0
                        + kernels[424] * -120.0
                        + kernels[425] * -120.0
                        + kernels[426] * -120.0
                        + kernels[427] * -120.0
                        + kernels[428] * -120.0
                        + kernels[429] * -120.0
                        + kernels[430] * -120.0
                        + kernels[431] * -120.0
                        + kernels[432] * -120.0
                        + kernels[433] * -120.0
                        + kernels[434] * -120.0
                        + kernels[435] * -120.0
                        + kernels[436] * -120.0
                        + kernels[437] * -120.0
                        + kernels[438] * -120.0
                        + kernels[439] * -120.0
                        + kernels[440] * -120.0
                        + kernels[441] * -120.0
                        + kernels[442] * -81.754137416873
                        + kernels[443] * -120.0
                        + kernels[444] * -120.0
                        + kernels[445] * -120.0
                        + kernels[446] * -120.0
                        + kernels[447] * -120.0
                        + kernels[448] * -120.0
                        + kernels[449] * -120.0
                        + kernels[450] * -120.0
                        + kernels[451] * -120.0
                        + kernels[452] * -120.0
                        + kernels[453] * -120.0
                        + kernels[454] * -120.0
                        + kernels[455] * -120.0
                        + kernels[456] * -120.0
                        + kernels[457] * -120.0
                        + kernels[458] * -120.0
                        + kernels[459] * -120.0
                        ;
                        decisions[5] = 1.427256648271
                        + kernels[82] * 120.0
                        + kernels[83] * 120.0
                        + kernels[84] * 120.0
                        + kernels[85] * 120.0
                        + kernels[87] * 120.0
                        + kernels[88] * 120.0
                        + kernels[90] * 120.0
                        + kernels[96] * 120.0
                        + kernels[98] * 120.0
                        + kernels[104] * 120.0
                        + kernels[105] * 31.334990315009
                        + kernels[129] * 120.0
                        + kernels[131] * 15.042321199245
                        + kernels[137] * 120.0
                        + kernels[148] * 114.514259684206
                        + kernels[149] * 120.0
                        + kernels[150] * 120.0
                        + kernels[151] * 117.265408275079
                        + kernels[154] * 120.0
                        + kernels[156] * 120.0
                        + kernels[157] * 120.0
                        + kernels[158] * 120.0
                        + kernels[163] * 120.0
                        + kernels[164] * 15.020381522837
                        + kernels[170] * 120.0
                        + kernels[171] * 120.0
                        + kernels[174] * 46.985183704485
                        + kernels[181] * 120.0
                        + kernels[183] * 120.0
                        + kernels[186] * 120.0
                        + kernels[200] * 120.0
                        + kernels[202] * 120.0
                        + kernels[203] * 120.0
                        + kernels[208] * 120.0
                        + kernels[221] * 120.0
                        + kernels[223] * 120.0
                        + kernels[224] * 46.974532493006
                        + kernels[228] * 120.0
                        + kernels[229] * 120.0
                        + kernels[231] * 120.0
                        + kernels[233] * 120.0
                        + kernels[235] * 120.0
                        + kernels[460] * -120.0
                        + kernels[461] * -120.0
                        + kernels[462] * -120.0
                        + kernels[463] * -120.0
                        + kernels[464] * -120.0
                        + kernels[465] * -120.0
                        + kernels[466] * -120.0
                        + kernels[467] * -120.0
                        + kernels[468] * -120.0
                        + kernels[469] * -120.0
                        + kernels[470] * -120.0
                        + kernels[471] * -120.0
                        + kernels[472] * -120.0
                        + kernels[473] * -120.0
                        + kernels[474] * -120.0
                        + kernels[475] * -120.0
                        + kernels[476] * -120.0
                        + kernels[477] * -120.0
                        + kernels[478] * -120.0
                        + kernels[479] * -120.0
                        + kernels[480] * -120.0
                        + kernels[481] * -120.0
                        + kernels[482] * -120.0
                        + kernels[483] * -120.0
                        + kernels[484] * -120.0
                        + kernels[485] * -120.0
                        + kernels[486] * -120.0
                        + kernels[487] * -120.0
                        + kernels[488] * -120.0
                        + kernels[489] * -120.0
                        + kernels[490] * -120.0
                        + kernels[491] * -120.0
                        + kernels[492] * -120.0
                        + kernels[493] * -27.137077193867
                        + kernels[495] * -120.0
                        + kernels[496] * -120.0
                        + kernels[497] * -120.0
                        + kernels[498] * -120.0
                        + kernels[499] * -120.0
                        ;
                        decisions[6] = 2.208931127033
                        + kernels[162] * 11.026856815561
                        + kernels[164] * 15.31305794865
                        + kernels[165] * 25.653483750414
                        + kernels[175] * 68.006601485375
                        + kernels[500] * -120.0
                        ;
                        decisions[7] = -0.13365251959
                        + kernels[236] * 120.0
                        + kernels[237] * 120.0
                        + kernels[238] * 120.0
                        + kernels[241] * 120.0
                        + kernels[248] * 120.0
                        + kernels[275] * 25.089697777922
                        + kernels[309] * 31.431869712441
                        + kernels[319] * 120.0
                        + kernels[325] * 120.0
                        + kernels[326] * 120.0
                        + kernels[327] * 120.0
                        + kernels[328] * 120.0
                        + kernels[331] * 109.957533530002
                        + kernels[334] * 120.0
                        + kernels[338] * 120.0
                        + kernels[339] * 66.205410065851
                        + kernels[340] * 120.0
                        + kernels[350] * 120.0
                        + kernels[351] * 109.033085082009
                        + kernels[355] * 120.0
                        + kernels[356] * 120.0
                        + kernels[357] * 120.0
                        + kernels[362] * 120.0
                        + kernels[399] * 120.0
                        + kernels[415] * 120.0
                        + kernels[420] * 120.0
                        + kernels[422] * 120.0
                        + kernels[431] * 120.0
                        + kernels[433] * 120.0
                        + kernels[435] * 120.0
                        + kernels[436] * 112.448546678945
                        + kernels[438] * 120.0
                        + kernels[442] * 120.0
                        + kernels[446] * 120.0
                        + kernels[447] * 120.0
                        + kernels[448] * 120.0
                        + kernels[449] * 120.0
                        + kernels[450] * 120.0
                        + kernels[451] * 42.588647530211
                        + kernels[452] * 120.0
                        + kernels[456] * 91.738547468324
                        + kernels[459] * 120.0
                        + kernels[460] * -120.0
                        + kernels[461] * -120.0
                        + kernels[462] * -120.0
                        + kernels[463] * -120.0
                        + kernels[464] * -120.0
                        + kernels[465] * -120.0
                        + kernels[466] * -120.0
                        + kernels[467] * -120.0
                        + kernels[468] * -120.0
                        + kernels[469] * -120.0
                        + kernels[470] * -120.0
                        + kernels[471] * -120.0
                        + kernels[472] * -120.0
                        + kernels[473] * -120.0
                        + kernels[474] * -120.0
                        + kernels[475] * -120.0
                        + kernels[476] * -120.0
                        + kernels[477] * -120.0
                        + kernels[478] * -120.0
                        + kernels[479] * -120.0
                        + kernels[480] * -120.0
                        + kernels[481] * -120.0
                        + kernels[482] * -120.0
                        + kernels[483] * -120.0
                        + kernels[484] * -120.0
                        + kernels[485] * -120.0
                        + kernels[486] * -120.0
                        + kernels[487] * -120.0
                        + kernels[488] * -120.0
                        + kernels[489] * -120.0
                        + kernels[490] * -120.0
                        + kernels[491] * -120.0
                        + kernels[492] * -120.0
                        + kernels[493] * -120.0
                        + kernels[494] * -108.493337845705
                        + kernels[495] * -120.0
                        + kernels[497] * -120.0
                        + kernels[498] * -120.0
                        + kernels[499] * -120.0
                        ;
                        decisions[8] = 2.055086775397
                        + kernels[331] * 72.880867981793
                        + kernels[335] * 24.423643297559
                        + kernels[342] * 22.695488720649
                        + kernels[500] * -120.0
                        ;
                        decisions[9] = 0.555204391344
                        + kernels[474] * 52.989478788174
                        + kernels[475] * 14.218348558683
                        + kernels[500] * -67.207827346857
                        ;
                        votes[decisions[0] > 0 ? 0 : 1] += 1;
                        votes[decisions[1] > 0 ? 0 : 2] += 1;
                        votes[decisions[2] > 0 ? 0 : 3] += 1;
                        votes[decisions[3] > 0 ? 0 : 4] += 1;
                        votes[decisions[4] > 0 ? 1 : 2] += 1;
                        votes[decisions[5] > 0 ? 1 : 3] += 1;
                        votes[decisions[6] > 0 ? 1 : 4] += 1;
                        votes[decisions[7] > 0 ? 2 : 3] += 1;
                        votes[decisions[8] > 0 ? 2 : 4] += 1;
                        votes[decisions[9] > 0 ? 3 : 4] += 1;
                        int val = votes[0];
                        int idx = 0;

                        for (int i = 1; i < 5; i++) {
                            if (votes[i] > val) {
                                val = votes[i];
                                idx = i;
                            }
                        }

                        return idx;
                    }

                protected:
                    /**
                    * Compute kernel between feature vector and support vector.
                    * Kernel type: rbf
                    */
                    float compute_kernel(float *x, ...) {
                        va_list w;
                        va_start(w, 6);
                        float kernel = 0.0;

                        for (uint16_t i = 0; i < 6; i++) {
                            kernel += pow(x[i] - va_arg(w, double), 2);
                        }

                        return exp(-0.0001 * kernel);
                    }
                };
            }
        }
    }