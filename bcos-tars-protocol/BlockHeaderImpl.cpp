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
 * @brief implementation for BlockHeader
 * @file BlockHeaderImpl.cpp
 * @author: ancelmo
 * @date 2021-04-20
 */
#include "BlockHeaderImpl.h"

using namespace bcostars;
using namespace bcostars::protocol;

void BlockHeaderImpl::decode(bcos::bytesConstRef _data)
{
    m_buffer.assign(_data.begin(), _data.end());

    tars::TarsInputStream<tars::BufferReader> input;
    input.setBuffer((const char*)_data.data(), _data.size());

    m_inner()->readFrom(input);
}

void BlockHeaderImpl::encode(bcos::bytes& _encodeData) const
{
    tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

    m_inner()->writeTo(output);
    output.getByteBuffer().swap(_encodeData);
}

bcos::bytesConstRef BlockHeaderImpl::encode(bool _onlyHashFieldsData) const
{
    if (_onlyHashFieldsData)
    {
        vector<Signature> emptyList;
        m_inner()->signatureList.swap(emptyList);
        encode(m_buffer);
        emptyList.swap(m_inner()->signatureList);
    }
    else
    {
        encode(m_buffer);
    }
    return bcos::ref(m_buffer);
}

void BlockHeaderImpl::clear()
{
    m_inner()->resetDefautlt();
    m_parentInfo.clear();
    m_gasUsed = 0;
    m_buffer.clear();
}

gsl::span<const bcos::protocol::ParentInfo> BlockHeaderImpl::parentInfo() const
{
    if (m_parentInfo.empty())
    {
        for (auto const& it : m_inner()->parentInfo)
        {
            bcos::protocol::ParentInfo parentInfo;
            parentInfo.blockNumber = it.blockNumber;
            parentInfo.blockHash =
                *(reinterpret_cast<const bcos::crypto::HashType*>(it.blockHash.data()));
            m_parentInfo.emplace_back(parentInfo);
        }
    }

    return gsl::span(m_parentInfo.data(), m_parentInfo.size());
}

bcos::crypto::HashType const& BlockHeaderImpl::txsRoot() const
{
    if (!m_inner()->txsRoot.empty())
    {
        return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner()->txsRoot.data()));
    }
    return bcostars::protocol::emptyHash;
}

bcos::crypto::HashType const& BlockHeaderImpl::stateRoot() const
{
    if (!m_inner()->stateRoot.empty())
    {
        return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner()->stateRoot.data()));
    }
    return bcostars::protocol::emptyHash;
}

bcos::crypto::HashType const& BlockHeaderImpl::receiptsRoot() const
{
    if (!m_inner()->receiptRoot.empty())
    {
        return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner()->receiptRoot.data()));
    }
    return bcostars::protocol::emptyHash;
}

bcos::u256 const& BlockHeaderImpl::gasUsed()
{
    if (!m_inner()->gasUsed.empty())
    {
        m_gasUsed = boost::lexical_cast<bcos::u256>(m_inner()->gasUsed);
    }
    return m_gasUsed;
}

void BlockHeaderImpl::setParentInfo(gsl::span<const bcos::protocol::ParentInfo> const& _parentInfo)
{
    m_parentInfo.clear();
    m_inner()->parentInfo.clear();
    for (auto& it : _parentInfo)
    {
        ParentInfo parentInfo;
        parentInfo.blockNumber = it.blockNumber;
        parentInfo.blockHash.assign(it.blockHash.begin(), it.blockHash.end());
        m_inner()->parentInfo.emplace_back(parentInfo);
    }
}

void BlockHeaderImpl::setSealerList(gsl::span<const bcos::bytes> const& _sealerList)
{
    m_inner()->sealerList.clear();
    for (auto const& it : _sealerList)
    {
        m_inner()->sealerList.push_back(std::vector<char>(it.begin(), it.end()));
    }
}

void BlockHeaderImpl::setSignatureList(
    gsl::span<const bcos::protocol::Signature> const& _signatureList)
{
    for (auto& it : _signatureList)
    {
        Signature signature;
        signature.sealerIndex = it.index;
        signature.signature.assign(it.signature.begin(), it.signature.end());
        m_inner()->signatureList.emplace_back(signature);
    }
}