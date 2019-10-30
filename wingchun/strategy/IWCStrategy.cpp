/*****************************************************************************
 * Copyright [2017] [taurus.ai]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

/**
 * IWCStrategy: base class of wingchun strategy.
 * @Author cjiang (changhao.jiang@taurus.ai)
 * @since   September, 2017
 */

#include "IWCStrategy.h"
#include <csignal>
#include "longfist/LFUtils.h"

USING_WC_NAMESPACE

void setup_signal_callback()
{
    std::signal(SIGTERM, IWCDataProcessor::signal_handler);
    std::signal(SIGINT, IWCDataProcessor::signal_handler);
    std::signal(SIGHUP, IWCDataProcessor::signal_handler);
    std::signal(SIGQUIT, IWCDataProcessor::signal_handler);
    std::signal(SIGKILL, IWCDataProcessor::signal_handler);
}

IWCStrategy::IWCStrategy(const string &name): name(name)
{
    logger = yijinjing::KfLog::getStrategyLogger(name, name);
    util = WCStrategyUtilPtr(new WCStrategyUtil(name));
    data = WCDataWrapperPtr(new WCDataWrapper(this, util.get()));
    setup_signal_callback();
}

/* start data thread */
void IWCStrategy::start()
{
    data_thread = ThreadPtr(new std::thread(&WCDataWrapper::run, data.get()));
    KF_LOG_INFO(logger, "[start] data started");
}

IWCStrategy::~IWCStrategy()
{
    KF_LOG_INFO(logger, "[~IWCStrategy]");
    data.reset();
    data_thread.reset();
    logger.reset();
    util.reset();
}

/* terminate data thread */
void IWCStrategy::terminate()
{
    stop();
    if (data_thread.get() != nullptr)
    {
        data_thread->join();
        data_thread.reset();
    }
    KF_LOG_INFO(logger, "[terminate] terminated!");
}

/* stop send stop signal to data thread */
void IWCStrategy::stop()
{
    if (data.get() != nullptr)
    {
        data->stop();
    }
}

void IWCStrategy::run()
{
    data->run();
}

/* block process by data thread */
void IWCStrategy::block()
{
    if (data_thread.get() != nullptr)
    {
        data_thread->join();
    }
}

void IWCStrategy::on_market_data(const LFMarketDataField* data, short source, long rcv_time)
{
    KF_LOG_DEBUG(logger, "[market_data] (source)" << source << " (ticker)" << data->InstrumentID << " (t)" << data->UpdateTime << "." << std::setfill('0') << std::setw(3) << data->UpdateMillisec << std::setfill(' ') << " (lp)" << data->LastPrice << " (v)" << data->Volume << " (to)" << data->Turnover << " (bid)" << data->BidVolume1 << "@" << data->BidPrice1 << " (ask)" << data->AskVolume1 << "@" << data->AskPrice1);
}

void IWCStrategy::on_market_data_level2(const LFL2MarketDataField* data, short source, long rcv_time)
{
    KF_LOG_DEBUG(logger, "[market_data_level2] (source)" << source << " (ticker)" << data->InstrumentID << " (lp)" << data->LastPrice);
}

void IWCStrategy::on_l2_index(const LFL2IndexField* data, short source, long rcv_time)
{
    KF_LOG_DEBUG(logger, "[l2_index] (source)" << source << " (ticker)" << data->InstrumentID << " (lp)" << data->LastIndex);
}

void IWCStrategy::on_l2_order(const LFL2OrderField* data, short source, long rcv_time)
{
    KF_LOG_DEBUG(logger, "[l2_order] (source)" << source << " (ticker)" << data->InstrumentID << " (p)" << data->Price << " (v)" << data->Volume);
}

void IWCStrategy::on_l2_trade(const LFL2TradeField* data, short source, long rcv_time)
{
    KF_LOG_DEBUG(logger, "[l2_trade] (source)" << source << " (ticker)" << data->InstrumentID << " (p)" << data->Price << " (v)" << data->Volume);
}

void IWCStrategy::on_rtn_order(const LFRtnOrderField* data, int request_id, short source, long rcv_time)
{
    KF_LOG_DEBUG(logger, "[rtn_order] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->LimitPrice << " (v_rmn)" << data->VolumeTotal << " (v_trd)" << data->VolumeTraded << " (direction)" << data->Direction << " (offset)" << data->OffsetFlag << " (status)" << data->OrderStatus);
}

void IWCStrategy::on_rtn_trade(const LFRtnTradeField* data, int request_id, short source, long rcv_time)
{
    KF_LOG_DEBUG(logger, "[rtn_trade] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->Price << " (v)" << data->Volume << " (direction)" << data->Direction << " (offset)" << data->OffsetFlag);
}

void IWCStrategy::on_rsp_order_insert(const LFInputOrderField* data, int request_id, short source, long rcv_time, short errorId, const char* errorMsg)
{
    if (errorId == 0)
        KF_LOG_DEBUG(logger, "[rsp_order_insert] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->LimitPrice << " (v)" << data->Volume << " (direction)" << data->Direction << " (offset)" << data->OffsetFlag);
    else
        KF_LOG_ERROR(logger, "[rsp_order_insert] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->LimitPrice << " (v)" << data->Volume << " (direction)" << data->Direction << " (offset)" << data->OffsetFlag << " (errId)" << errorId << " (errMsg)" << errorMsg);
}

void IWCStrategy::on_rsp_order_action(const LFOrderActionField* data, int request_id, short source, long rcv_time, short errorId, const char* errorMsg)
{
    if (errorId == 0)
        KF_LOG_DEBUG(logger, "[rsp_order_action] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->LimitPrice << " (v)" << data->VolumeChange << " (action)" << data->ActionFlag);
    else
        KF_LOG_ERROR(logger, "[rsp_order_action] (source)" << source << " (rid)" << request_id << " (ticker)" << data->InstrumentID << " (p)" << data->LimitPrice << " (v)" << data->VolumeChange << " (action)" << data->ActionFlag << " (errId)" << errorId << " (errMsg)" << errorMsg);
}

void IWCStrategy::on_rsp_position(const PosHandlerPtr posMap, int request_id, short source, long rcv_time)
{
    KF_LOG_DEBUG(logger, "[rsp_position] (source)" << source << " (rid)" << request_id);
    posMap->print(logger);
}

void IWCStrategy::on_market_bar(const BarMdMap& data, int min_interval, short source, long rcv_time)
{
    for (auto &iter: data)
    {
        const LFBarMarketDataField& bar = iter.second;
        KF_LOG_DEBUG(logger, "[market_bar] (ticker)" << iter.first << " (o)" << bar.Open << " (h)" << bar.High << " (l)" << bar.Low << " (c)" << bar.Close);
    }
}

/* system utilities */
void IWCStrategy::on_switch_day(long rcv_time)
{
    KF_LOG_DEBUG(logger, "[switch_day] (nano)" << rcv_time);
}

void IWCStrategy::on_time(long cur_time)
{
    util->process_callback(cur_time);
}

void IWCStrategy::on_td_login(bool ready, const json& js, short source)
{
    KF_LOG_DEBUG(logger, "[td_login] (source)" << source << " (ready)" << ready << " (pos)" << js.dump());
}

bool IWCStrategy::td_is_ready(short source) const
{
    if (data.get() != nullptr)
    {
        byte status = data->get_td_status(source);
        if (status == CONNECT_TD_STATUS_ACK_POS
            || status == CONNECT_TD_STATUS_SET_BACK)
            return true;
    }
    return false;
}

bool IWCStrategy::td_is_connected(short source) const
{
    if (data.get() != nullptr)
    {
        byte status = data->get_td_status(source);
        if (status == CONNECT_TD_STATUS_ACK_POS
            || status == CONNECT_TD_STATUS_SET_BACK
            || status == CONNECT_TD_STATUS_ACK_NONE)
            return true;
    }
    return false;
}

#define CHECK_TD_READY(source) \
    if (!td_is_ready(source)) \
    {\
        if (!td_is_connected(source))\
            KF_LOG_ERROR(logger, "td (" << source << ") is not connected. please check TD or yjj status"); \
        else \
            KF_LOG_ERROR(logger, "td (" << source << ") holds no position here. please init current strategy's position before insert an order"); \
        return -1;\
    }

#define CHECK_EXCHANGE_AND_OFFSET(exchange_name, offset) \
    {\
        short eid = getExchangeId(exchange_name);\
        if (eid < 0){\
            KF_LOG_ERROR(logger, "unrecognized exchange name: " << exchange_name); \
            return -1;\
        }\
        if (eid == EXCHANGE_ID_CFFEX || eid == EXCHANGE_ID_DCE || eid == EXCHANGE_ID_CZCE){\
            if (offset == LF_CHAR_CloseToday || offset == LF_CHAR_CloseYesterday){\
                KF_LOG_DEBUG(logger, "CFFEX/DCE/CZCE don't support CloseToday or CloseYesterday, auto revised to default Close");\
                offset = LF_CHAR_Close;\
            }\
        }\
        if (eid == EXCHANGE_ID_SSE || eid == EXCHANGE_ID_SZE) {\
            if (offset != LF_CHAR_Open) {\
                KF_LOG_DEBUG(logger, "stock don't need to specify offset, default is open");\
                offset = LF_CHAR_Open;\
            }\
        }\
    }

/** util functions, check before calling WCStrategyUtil */
int IWCStrategy::insert_market_order(short source, string instrument_id, string exchange_id, int volume, LfDirectionType direction, LfOffsetFlagType offset)
{
    CHECK_TD_READY(source);
    CHECK_EXCHANGE_AND_OFFSET(exchange_id, offset);
    int rid = util->insert_market_order(source, instrument_id, exchange_id, volume, direction, offset);
    KF_LOG_DEBUG(logger, "[insert_market_order] (source)" << source << " (rid)" << rid << " (ticker)" << instrument_id << " (exchange)" << exchange_id << " (v)" << volume << " (direction)" << direction << " (offset)" << offset);
    return rid;
}

int IWCStrategy::insert_limit_order(short source, string instrument_id, string exchange_id, double price, int volume, LfDirectionType direction, LfOffsetFlagType offset)
{
    CHECK_TD_READY(source);
    CHECK_EXCHANGE_AND_OFFSET(exchange_id, offset);
    int rid = util->insert_limit_order(source, instrument_id, exchange_id, price, volume, direction, offset);
    KF_LOG_DEBUG(logger, "[insert_limit_order] (source)" << source << " (rid)" << rid << " (ticker)" << instrument_id << " (exchange)" << exchange_id << " (p)" << price << " (v)" << volume << " (direction)" << direction << " (offset)" << offset);
    return rid;
}

int IWCStrategy::insert_fok_order(short source, string instrument_id, string exchange_id, double price, int volume, LfDirectionType direction, LfOffsetFlagType offset)
{
    CHECK_TD_READY(source);
    CHECK_EXCHANGE_AND_OFFSET(exchange_id, offset);
    int rid = util->insert_fok_order(source, instrument_id, exchange_id, price, volume, direction, offset);
    KF_LOG_DEBUG(logger, "[insert_fok_order] (source)" << source << " (rid)" << rid << " (ticker)" << instrument_id << " (exchange)" << exchange_id << " (p)" << price << " (v)" << volume << " (direction)" << direction << " (offset)" << offset);
    return rid;
}

int IWCStrategy::insert_fak_order(short source, string instrument_id, string exchange_id, double price, int volume, LfDirectionType direction, LfOffsetFlagType offset)
{
    CHECK_TD_READY(source);
    CHECK_EXCHANGE_AND_OFFSET(exchange_id, offset);
    int rid = util->insert_fak_order(source, instrument_id, exchange_id, price, volume, direction, offset);
    KF_LOG_DEBUG(logger, "[insert_fak_order] (source)" << source << " (rid)" << rid << " (ticker)" << instrument_id << " (exchange)" << exchange_id << " (p)" << price << " (v)" << volume << " (direction)" << direction << " (offset)" << offset);
    return rid;
}

int IWCStrategy::req_position(short source)
{
    if (!td_is_connected(source))
    {
        KF_LOG_ERROR(logger, "td (" << source << ") connection is failed. please check TD or yjj status");
        return -1;
    }
    int rid = util->req_position(source);
    KF_LOG_DEBUG(logger, "[req_position] (source)" << source);
    return rid;
}

int IWCStrategy::cancel_order(short source, int order_id)
{
    CHECK_TD_READY(source);
    int rid = util->cancel_order(source, order_id);
    KF_LOG_DEBUG(logger, "[cancel_order] (source)" << source << " (rid)" << rid << " (order_id)" << order_id);
    return rid;
}
