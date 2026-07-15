#pragma once
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include "minikv/slice.h"
#include "utils/hash.h"

namespace minikv {
namespace core {

class BloomFilter {
public:
    BloomFilter(size_t expected_keys, double false_positive_rate = 0.01)
        : bits_per_key_(0), num_hashes_(0) {
        int bits = static_cast<int>(-1.0 * std::log(false_positive_rate) / std::log(2.0) / std::log(2.0));
        num_hashes_ = static_cast<int>(bits * std::log(2.0));
        if (num_hashes_ < 1) num_hashes_ = 1;
        if (num_hashes_ > 30) num_hashes_ = 30;
        bits_per_key_ = static_cast<int>(static_cast<double>(bits) / std::log(2.0));
        if (bits_per_key_ < 1) bits_per_key_ = 1;
        bits_.assign(expected_keys * bits_per_key_ / 8 + 1, 0);
    }

    void add(const Slice& key) {
        uint32_t h1 = utils::murmurHash2(key.data(), key.size(), 0xbc9f1d34);
        uint32_t h2 = utils::murmurHash2(key.data(), key.size(), 0x9e3779b9);
        uint32_t bits = static_cast<uint32_t>(bits_.size() * 8);
        for (int i = 0; i < num_hashes_; ++i) {
            uint32_t pos = (h1 + i * h2) % bits;
            bits_[pos / 8] |= (1 << (pos % 8));
        }
    }

    bool mightContain(const Slice& key) const {
        uint32_t h1 = utils::murmurHash2(key.data(), key.size(), 0xbc9f1d34);
        uint32_t h2 = utils::murmurHash2(key.data(), key.size(), 0x9e3779b9);
        uint32_t bits = static_cast<uint32_t>(bits_.size() * 8);
        for (int i = 0; i < num_hashes_; ++i) {
            uint32_t pos = (h1 + i * h2) % bits;
            if ((bits_[pos / 8] & (1 << (pos % 8))) == 0) return false;
        }
        return true;
    }

    void persist(const std::string& path) const;
    static std::unique_ptr<BloomFilter> load(const std::string& path);

    size_t memoryUsage() const { return bits_.size(); }
    int numHashes() const { return num_hashes_; }

private:
    int bits_per_key_;
    int num_hashes_;
    std::vector<uint8_t> bits_;
};

}  // namespace core
}  // namespace minikv
