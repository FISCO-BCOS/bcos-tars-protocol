/**
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @file Common.h
 * @author: ancelmo
 * @date 2021-04-20
 */

#pragma once

#include "bcos-tars-protocol/tars/GroupInfo.h"
#include "bcos-tars-protocol/tars/LedgerConfig.h"
#include <bcos-framework/interfaces/consensus/ConsensusNode.h>
#include <bcos-framework/interfaces/crypto/Hash.h>
#include <bcos-framework/interfaces/crypto/KeyFactory.h>
#include <bcos-framework/interfaces/ledger/LedgerConfig.h>
#include <bcos-framework/interfaces/multigroup/ChainNodeInfoFactory.h>
#include <bcos-framework/interfaces/multigroup/GroupInfoFactory.h>
#include <bcos-framework/libutilities/Common.h>
#include <tarscpp/tup/Tars.h>
#include <cstdint>
#include <functional>
#include <memory>

namespace bcostars
{
namespace protocol
{
static bcos::crypto::HashType emptyHash;

class BufferWriterByteVector
{
protected:
    mutable std::vector<bcos::byte> _buffer;
    bcos::byte* _buf;
    std::size_t _len;
    std::size_t _buf_len;
    std::function<bcos::byte*(BufferWriterByteVector&, size_t)> _reserve;

private:
    BufferWriterByteVector(const BufferWriterByteVector&);
    BufferWriterByteVector& operator=(const BufferWriterByteVector& buf);

public:
    BufferWriterByteVector() : _buf(NULL), _len(0), _buf_len(0)
    {
#ifndef GEN_PYTHON_MASK
        _reserve = [](BufferWriterByteVector& os, size_t len) {
            os._buffer.resize(len);
            return os._buffer.data();
        };
#endif
    }

    ~BufferWriterByteVector() {}

    void reset() { _len = 0; }

    void writeBuf(const bcos::byte* buf, size_t len)
    {
        TarsReserveBuf(*this, _len + len);
        memcpy(_buf + _len, buf, len);
        _len += len;
    }

    const std::vector<bcos::byte>& getByteBuffer() const
    {
        _buffer.resize(_len);
        return _buffer;
    }
    std::vector<bcos::byte>& getByteBuffer()
    {
        _buffer.resize(_len);
        return _buffer;
    }
    const bcos::byte* getBuffer() const { return _buf; }
    size_t getLength() const { return _len; }
    void swap(std::vector<bcos::byte>& v)
    {
        _buffer.resize(_len);
        v.swap(_buffer);
        _buf = NULL;
        _buf_len = 0;
        _len = 0;
    }
    void swap(BufferWriterByteVector& buf)
    {
        buf._buffer.swap(_buffer);
        std::swap(_buf, buf._buf);
        std::swap(_buf_len, buf._buf_len);
        std::swap(_len, buf._len);
    }
};
}  // namespace protocol

inline bcos::group::ChainNodeInfo::Ptr toBcosChainNodeInfo(
    bcos::group::ChainNodeInfoFactory::Ptr _factory, bcostars::ChainNodeInfo const& _tarsNodeInfo)
{
    auto nodeInfo = _factory->createNodeInfo();
    nodeInfo->setNodeName(_tarsNodeInfo.nodeName);
    nodeInfo->setNodeType((bcos::group::NodeType)_tarsNodeInfo.nodeType);
    nodeInfo->setNodeID(_tarsNodeInfo.nodeID);
    nodeInfo->setIniConfig(_tarsNodeInfo.iniConfig);
    nodeInfo->setMicroService(_tarsNodeInfo.microService);
    for (auto const& it : _tarsNodeInfo.serviceInfo)
    {
        nodeInfo->appendServiceInfo((bcos::protocol::ServiceType)it.first, it.second);
    }
    return nodeInfo;
}

inline bcos::group::GroupInfo::Ptr toBcosGroupInfo(
    bcos::group::ChainNodeInfoFactory::Ptr _nodeFactory,
    bcos::group::GroupInfoFactory::Ptr _groupFactory, bcostars::GroupInfo const& _tarsGroupInfo)
{
    auto groupInfo = _groupFactory->createGroupInfo();
    groupInfo->setChainID(_tarsGroupInfo.chainID);
    groupInfo->setGroupID(_tarsGroupInfo.groupID);
    groupInfo->setGenesisConfig(_tarsGroupInfo.genesisConfig);
    groupInfo->setIniConfig(_tarsGroupInfo.iniConfig);
    for (auto const& tarsNodeInfo : _tarsGroupInfo.nodeList)
    {
        groupInfo->appendNodeInfo(toBcosChainNodeInfo(_nodeFactory, tarsNodeInfo));
    }
    return groupInfo;
}

inline bcostars::ChainNodeInfo toTarsChainNodeInfo(bcos::group::ChainNodeInfo::Ptr _nodeInfo)
{
    bcostars::ChainNodeInfo tarsNodeInfo;
    if (!_nodeInfo)
    {
        return tarsNodeInfo;
    }
    tarsNodeInfo.nodeName = _nodeInfo->nodeName();
    tarsNodeInfo.nodeType = _nodeInfo->nodeType();
    auto const& info = _nodeInfo->serviceInfo();
    for (auto const& it : info)
    {
        tarsNodeInfo.serviceInfo[(int32_t)it.first] = it.second;
    }
    tarsNodeInfo.nodeID = _nodeInfo->nodeID();
    tarsNodeInfo.microService = _nodeInfo->microService();
    tarsNodeInfo.iniConfig = _nodeInfo->iniConfig();
    return tarsNodeInfo;
}

inline bcostars::GroupInfo toTarsGroupInfo(bcos::group::GroupInfo::Ptr _groupInfo)
{
    bcostars::GroupInfo tarsGroupInfo;
    if (!_groupInfo)
    {
        return tarsGroupInfo;
    }
    tarsGroupInfo.chainID = _groupInfo->chainID();
    tarsGroupInfo.groupID = _groupInfo->groupID();
    tarsGroupInfo.genesisConfig = _groupInfo->genesisConfig();
    tarsGroupInfo.iniConfig = _groupInfo->iniConfig();
    // set nodeList
    std::vector<bcostars::ChainNodeInfo> tarsNodeList;
    auto bcosNodeList = _groupInfo->nodeInfos();
    for (auto const& it : bcosNodeList)
    {
        auto const& nodeInfo = it.second;
        tarsNodeList.emplace_back(toTarsChainNodeInfo(nodeInfo));
    }
    tarsGroupInfo.nodeList = std::move(tarsNodeList);
    return tarsGroupInfo;
}

inline bcos::consensus::ConsensusNodeListPtr toConsensusNodeList(
    bcos::crypto::KeyFactory::Ptr _keyFactory,
    vector<bcostars::ConsensusNode> const& _tarsConsensusNodeList)
{
    auto consensusNodeList = std::make_shared<bcos::consensus::ConsensusNodeList>();
    for (auto const& node : _tarsConsensusNodeList)
    {
        auto nodeID = _keyFactory->createKey(
            bcos::bytesConstRef((bcos::byte*)node.nodeID.data(), node.nodeID.size()));
        consensusNodeList->push_back(
            std::make_shared<bcos::consensus::ConsensusNode>(nodeID, node.weight));
    }
    return consensusNodeList;
}

inline bcos::ledger::LedgerConfig::Ptr toLedgerConfig(
    bcostars::LedgerConfig const& _ledgerConfig, bcos::crypto::KeyFactory::Ptr _keyFactory)
{
    auto ledgerConfig = std::make_shared<bcos::ledger::LedgerConfig>();
    auto consensusNodeList = toConsensusNodeList(_keyFactory, _ledgerConfig.consensusNodeList);
    ledgerConfig->setConsensusNodeList(*consensusNodeList);

    auto observerNodeList = toConsensusNodeList(_keyFactory, _ledgerConfig.observerNodeList);
    ledgerConfig->setObserverNodeList(*observerNodeList);

    auto hash = bcos::crypto::HashType();
    if (_ledgerConfig.hash.size() >= bcos::crypto::HashType::size)
    {
        hash = bcos::crypto::HashType(
            (const bcos::byte*)_ledgerConfig.hash.data(), bcos::crypto::HashType::size);
    }
    ledgerConfig->setHash(hash);
    ledgerConfig->setBlockNumber(_ledgerConfig.blockNumber);
    ledgerConfig->setBlockTxCountLimit(_ledgerConfig.blockTxCountLimit);
    ledgerConfig->setLeaderSwitchPeriod(_ledgerConfig.leaderSwitchPeriod);
    ledgerConfig->setSealerId(_ledgerConfig.sealerId);
    return ledgerConfig;
}

inline vector<bcostars::ConsensusNode> toTarsConsensusNodeList(
    bcos::consensus::ConsensusNodeList const& _nodeList)
{
    // set consensusNodeList
    vector<bcostars::ConsensusNode> tarsConsensusNodeList;
    for (auto node : _nodeList)
    {
        bcostars::ConsensusNode consensusNode;
        consensusNode.nodeID.assign(node->nodeID()->data().begin(), node->nodeID()->data().end());
        consensusNode.weight = node->weight();
        tarsConsensusNodeList.emplace_back(consensusNode);
    }
    return tarsConsensusNodeList;
}
inline bcostars::LedgerConfig toTarsLedgerConfig(bcos::ledger::LedgerConfig::Ptr _ledgerConfig)
{
    bcostars::LedgerConfig ledgerConfig;
    auto hash = _ledgerConfig->hash().asBytes();
    ledgerConfig.hash.assign(hash.begin(), hash.end());
    ledgerConfig.blockNumber = _ledgerConfig->blockNumber();
    ledgerConfig.blockTxCountLimit = _ledgerConfig->blockTxCountLimit();
    ledgerConfig.leaderSwitchPeriod = _ledgerConfig->leaderSwitchPeriod();
    ledgerConfig.sealerId = _ledgerConfig->sealerId();

    // set consensusNodeList
    ledgerConfig.consensusNodeList = toTarsConsensusNodeList(_ledgerConfig->consensusNodeList());
    // set observerNodeList
    ledgerConfig.observerNodeList = toTarsConsensusNodeList(_ledgerConfig->observerNodeList());
    return ledgerConfig;
}
}  // namespace bcostars