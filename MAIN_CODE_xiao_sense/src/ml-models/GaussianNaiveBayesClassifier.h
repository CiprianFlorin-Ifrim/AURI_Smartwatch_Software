#pragma once
#include <cstdarg>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class GaussianNB {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        float votes[5] = { 0.0f };
                        float theta[5] = { 0 };
                        float sigma[5] = { 0 };
                        theta[0] = 12.281596058344; theta[1] = 11.427239610978; theta[2] = 6.36166031986; theta[3] = 70.03574517004; theta[4] = 1019.817306079762;
                        sigma[0] = 40.391638839882; sigma[1] = 53.273660572583; sigma[2] = 25.354504499471; sigma[3] = 102.0906611003; sigma[4] = 67.569805585721;
                        votes[0] = 0.18549217184 - gauss(x, theta, sigma);
                        theta[0] = 11.024640766365; theta[1] = 9.937280468334; theta[2] = 7.546008515168; theta[3] = 80.531950505588; theta[4] = 1010.83145290048;
                        sigma[0] = 22.881773984094; sigma[1] = 34.21910467631; sigma[2] = 18.984886507428; sigma[3] = 70.060369656728; sigma[4] = 108.844356209247;
                        votes[1] = 0.203586326453 - gauss(x, theta, sigma);
                        theta[0] = 12.129235480333; theta[1] = 11.27537084013; theta[2] = 7.134024080593; theta[3] = 73.539651875678; theta[4] = 1018.946857490247;
                        sigma[0] = 31.701890626031; sigma[1] = 43.753759568157; sigma[2] = 22.831219130826; sigma[3] = 85.441190917404; sigma[4] = 64.634454639322;
                        votes[2] = 0.206728425158 - gauss(x, theta, sigma);
                        theta[0] = 8.966450614964; theta[1] = 7.692920689844; theta[2] = 5.847108776515; theta[3] = 81.496234531247; theta[4] = 1022.207379852971;
                        sigma[0] = 15.602057953906; sigma[1] = 24.96960519108; sigma[2] = 14.908865030819; sigma[3] = 57.678982359242; sigma[4] = 76.288627877136;
                        votes[3] = 0.200769272442 - gauss(x, theta, sigma);
                        theta[0] = -0.934688716618; theta[1] = -3.863304072659; theta[2] = -3.006491584202; theta[3] = 86.295271238847; theta[4] = 1014.79271114532;
                        sigma[0] = 0.529876120644; sigma[1] = 2.166698627443; sigma[2] = 1.012087078911; sigma[3] = 20.029039065493; sigma[4] = 213.402588841304;
                        votes[4] = 0.203423804106 - gauss(x, theta, sigma);
                        // return argmax of votes
                        uint8_t classIdx = 0;
                        float maxVotes = votes[0];

                        for (uint8_t i = 1; i < 5; i++) {
                            if (votes[i] > maxVotes) {
                                classIdx = i;
                                maxVotes = votes[i];
                            }
                        }

                        return classIdx;
                    }

                protected:
                    /**
                    * Compute gaussian value
                    */
                    float gauss(float *x, float *theta, float *sigma) {
                        float gauss = 0.0f;

                        for (uint16_t i = 0; i < 5; i++) {
                            gauss += log(sigma[i]);
                            gauss += abs(x[i] - theta[i]) / sigma[i];
                        }

                        return gauss;
                    }
                };
            }
        }
    }