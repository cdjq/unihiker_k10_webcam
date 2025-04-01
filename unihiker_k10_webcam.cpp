/*!
 * @file unihiker_k10_webcam.cpp
 * @brief 这是一个网络摄像头的驱动的库
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version  V1.0
 * @date  2025-03-21 
 * @url https://github.com/DFRobot/unihiker_k10_webcam
 */
#include "unihiker_k10_webcam.h"
#include "app_httpd.hpp"
#include "esp_http_server.h"

extern QueueHandle_t xQueueCamer;
static httpd_handle_t camera_httpd = NULL;
static camera_fb_t *frame = NULL;
esp_err_t stream_handler(httpd_req_t *req) {
    esp_err_t res = ESP_OK;
    
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char part_buf[128];

    res = httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");
    if (res != ESP_OK) {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    while (true) {
        // 从队列接收帧数据
        if (xQueueReceive(xQueueCamer, &frame, portMAX_DELAY) != pdTRUE) {
            DBG("continue");
            continue;  // 如果接收失败，继续等待
        }

        // 转换帧为 JPEG
        if (!frame2jpg(frame, 80, &_jpg_buf, &_jpg_buf_len)) {
            DBG("JPEG compression failed");
            esp_camera_fb_return(frame);
            res = ESP_FAIL;
            break;
        }

        // 释放帧数据
        esp_camera_fb_return(frame);
        

        size_t hlen = snprintf(part_buf, 128, "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %zu\r\n\r\n", _jpg_buf_len);

        res = httpd_resp_send_chunk(req, part_buf, hlen);

        // 发送 JPEG 数据
        res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        if (res != ESP_OK) break;

        // 结束当前帧
        res = httpd_resp_send_chunk(req, "\r\n", 2);
        if (res != ESP_OK) break;

        free(_jpg_buf);
        if (res != ESP_OK) break;
    }
    
    return res;
}

esp_err_t capture_handler(httpd_req_t *req) {
    DBG("Capture handler triggered!");
    esp_err_t res = ESP_OK;

    
    if (xQueueReceive(xQueueCamer, &frame, portMAX_DELAY) != pdTRUE) {
        DBG("continue");
        return httpd_resp_send_500(req);
    }

    size_t jpg_buf_len = 0;
    uint8_t *jpg_buf = NULL;

    // 转换为 JPEG 格式
    if (!frame2jpg(frame, 80, &jpg_buf, &jpg_buf_len)) {
        DBG("JPEG compression failed");
        esp_camera_fb_return(frame);
        return httpd_resp_send_500(req);
    }

    esp_camera_fb_return(frame);
    

    // 设置 HTTP 响应头
    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=capture.jpg");

    // 发送图片数据
    res = httpd_resp_send(req, (const char *)jpg_buf, jpg_buf_len);

    free(jpg_buf);
    return res;
}

const char index_html[] = R"rawliteral(
    <!DOCTYPE html>
    <html lang="zh">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>K10 摄像头</title>
    </head>
    <body>
        <h1>K10 摄像头</h1>
        <br>
        <button id="captureBtn">拍照并下载</button>
    
        <script>
            document.getElementById("captureBtn").addEventListener("click", () => {
                fetch("/capture")
                    .then(response => response.blob())
                    .then(blob => {
                    const url = URL.createObjectURL(blob);
                    const link = document.createElement("a");
                    link.href = url;
                    link.download = "esp32_snapshot.jpg"; // 强制指定文件名
                    document.body.appendChild(link);
                    link.click();
                    document.body.removeChild(link);
                    URL.revokeObjectURL(url);
                })
                .catch(error => console.error("Error downloading image:", error));
            });

        </script>
    </body>
    </html>
    )rawliteral";

esp_err_t index_handler(httpd_req_t *req) {
    DBG("Handling index request...");
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
}

unihiker_k10_webcam::unihiker_k10_webcam(void)
{

}

bool unihiker_k10_webcam::enableWebcam(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
     // 注册主页
     httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL
    };

    //注册视频流
    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL
    };

    // 注册单张图片捕获
    httpd_uri_t capture_uri = {
        .uri = "/capture",
        .method = HTTP_GET,
        .handler = capture_handler,
        .user_ctx = NULL
    };

    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        DBG("HTTP server started successfully");
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &stream_uri);
        httpd_register_uri_handler(camera_httpd, &capture_uri);
    }else{
        DBG("Failed to start HTTP server");
        return false;
    }
    return true;
}


bool unihiker_k10_webcam::disableWebcam(void)
{
    if(camera_httpd){
        httpd_stop(camera_httpd);  // 停止 HTTP 服务器
        camera_httpd = NULL;
        if(frame){
            esp_camera_fb_return(frame);
            frame =  NULL;
        }
        DBG("HTTP server stopped");
        return true;
    }
    return false;
}
