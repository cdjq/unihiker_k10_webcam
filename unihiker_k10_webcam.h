/*!
 * @file unihiker_k10_webcam.h
 * @brief 这是一个网络摄像头的驱动的库
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version  V1.0
 * @date  2025-03-21 
 * @url https://github.com/DFRobot/unihiker_k10_webcam
 */
#ifndef _UNIHIKER_K10_WEBCAM_H_
#define _UNIHIKER_K10_WEBCAM_H_
#include "Arduino.h"
#include "unihiker_k10.h"

#define ENABLE_DBG ///< Enable this macro to view the detailed execution process of the program.
#ifdef ENABLE_DBG
#define DBG(...) {Serial.print("[");Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

class unihiker_k10_webcam
{
public:
    /**
     * @fn unihiker_k10_webcam
     * @brief 这是网络摄像头的类
     */
    unihiker_k10_webcam(void);
    /**
        @fn enableWebcam
        @brief 开启网络摄像头
        @return 成功返回1，不成功返回0
     */
    bool enableWebcam(void);

    /**
        @fn disableWebcam
        @brief 关闭网络摄像头
        @return 成功返回1，不成功返回0
     */
    bool disableWebcam(void);



};
#endif

