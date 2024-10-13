
#include <iostream>
#include <iomanip>

#include "driver/rmt_rx.h"
#include "driver/rmt_types.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/rmt_types.h"
#include "portmacro.h"
#include "soc/clk_tree_defs.h"

namespace IRReceiver
{
    rmt_channel_handle_t channel_handle;
    rmt_rx_channel_config_t channel_config;
    QueueHandle_t receive_queue;
    rmt_rx_event_callbacks_t cbs;
    rmt_receive_config_t receive_config;
    rmt_symbol_word_t raw_symbols[64];
    rmt_rx_done_event_data_t rx_data;

    static bool example_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
    {
        BaseType_t high_task_wakeup = pdFALSE;
        QueueHandle_t receive_queue = (QueueHandle_t)user_data;
        // send the received RMT symbols to the parser task
        xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
        // return whether any task is woken up
        return high_task_wakeup == pdTRUE;
    }

    void init()
    {
        channel_handle = NULL;
        channel_config = {
            .gpio_num = GPIO_NUM_6,
            .clk_src = RMT_CLK_SRC_DEFAULT,
            .resolution_hz = 1 * 1000 * 1000,
            .mem_block_symbols = 64,
            .flags = {
                .invert_in = true,
            }
        };

        ESP_ERROR_CHECK(rmt_new_rx_channel(&channel_config, &channel_handle));
        ESP_ERROR_CHECK(rmt_enable(channel_handle));

        receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
        cbs = {
            .on_recv_done = example_rmt_rx_done_callback
        };

        ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(channel_handle, &cbs, receive_queue));

        receive_config = {
            .signal_range_min_ns = 1250,
            .signal_range_max_ns = 12 * 1000 * 1000
        };
    }

    void receive()
    {
        ESP_ERROR_CHECK(rmt_receive(channel_handle, raw_symbols, sizeof(raw_symbols), &receive_config));
        xQueueReceive(receive_queue, &rx_data, portMAX_DELAY);

        for (int i = 0; i < rx_data.num_symbols; i++)
        {
            rmt_symbol_word_t word = rx_data.received_symbols[i];
            std::cout << std::hex << std::noshowbase << std::setw(8) << std::setfill('0') << word.val;
        }

        std::cout << std::endl << std::endl;
    }
}