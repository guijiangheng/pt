#ifndef PT_CORE_DISTRIB_H
#define PT_CORE_DISTRIB_H

#include <stack>
#include <vector>
#include <numeric>
#include <pt/pt.h>

namespace pt {

class Distribution1D {
public:
    Distribution1D(std::vector<Float>&& pdf) : aliasIndex(pdf.size()) {
        auto sumInv = 1 / std::accumulate(pdf.cbegin(), pdf.cend(), (Float)0);
        for (auto& p : pdf) p *= sumInv;
        normalizedPdf = std::move(pdf);

        auto n = count();
        aliasPdf.reserve(n);
        for (auto p : normalizedPdf) aliasPdf.push_back(p * n);

        std::stack<int> low, high;
        for (auto i = 0; i < n; ++i) {
            if (aliasPdf[i] < 1) low.push(i);
            else if (aliasPdf[i] > 1) high.push(i);
        }

        while (!low.empty()) {
            auto l = low.top(), h = high.top();
            low.pop();
            aliasIndex[l] = h;
            aliasPdf[h] = aliasPdf[l] + aliasPdf[h] - 1;
            if (aliasPdf[h] == 1) high.pop();
            else if (aliasPdf[h] < 1) {
                high.pop();
                low.push(h);
            }
        }
    }

    int count() const {
        return (int)normalizedPdf.size();
    }

    Float sampleContinuous(Float u, Float& pdf, int& index) const {
        auto n = count();
        u *= n;
        index = (int)u;
        auto uRemapped = u - index;
        if (uRemapped < aliasPdf[index]) uRemapped /= aliasPdf[index];
        else {
            uRemapped = (uRemapped - aliasPdf[index]) / (1 - aliasPdf[index]);
            index = aliasIndex[index];
        }
        pdf = normalizedPdf[index] * n;
        return (index + uRemapped) / n;
    }

    int sampleDiscrete(Float u, Float& pdf) const {
        auto n = count();
        u *= n;
        auto index = (int)u;
        auto uRemapped = u - index;
        if (uRemapped > aliasPdf[index]) index = aliasIndex[index];
        pdf = normalizedPdf[index];
        return index;
    }

    Float discretePdf(int index) const {
        return normalizedPdf[index];
    }

private:
    std::vector<int> aliasIndex;
    std::vector<Float> aliasPdf;
    std::vector<Float> normalizedPdf;
};

}

#endif
