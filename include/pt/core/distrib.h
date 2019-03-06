#ifndef PT_CORE_DISTRIB_H
#define PT_CORE_DISTRIB_H

#include <stack>
#include <vector>
#include <numeric>
#include <pt/pt.h>
#include <pt/math/vector2.h>

namespace pt {

class Distribution1D {
public:
    Distribution1D() noexcept;

    Distribution1D(const Float* func, int n) noexcept : aliasIndex(n) {
        funcIntegral = std::accumulate(func, func + n, (Float)0);
        auto funcIntegralInv = 1 / funcIntegral;
        pdf.reserve(n);
        for (auto i = 0; i < n; ++i) pdf.push_back(funcIntegralInv * func[i]);

        aliasPdf.reserve(n);
        for (auto p : pdf) aliasPdf.push_back(p * n);

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
        return (int)pdf.size();
    }

    Float sampleContinuous(Float u, Float& p, int& index) const {
        auto n = count();
        u *= n;
        index = (int)u;
        auto uRemapped = u - index;
        if (uRemapped < aliasPdf[index]) uRemapped /= aliasPdf[index];
        else {
            uRemapped = (uRemapped - aliasPdf[index]) / (1 - aliasPdf[index]);
            index = aliasIndex[index];
        }
        p = pdf[index] * n;
        return (index + uRemapped) / n;
    }

    int sampleDiscrete(Float u, Float& p) const {
        auto n = count();
        u *= n;
        auto index = (int)u;
        auto uRemapped = u - index;
        if (uRemapped > aliasPdf[index]) index = aliasIndex[index];
        p = pdf[index];
        return index;
    }

    Float discretePdf(int index) const {
        return pdf[index];
    }

private:
    friend class Distribution2D;
    Float funcIntegral;
    std::vector<Float> pdf;
    std::vector<Float> aliasPdf;
    std::vector<int> aliasIndex;
};

class Distribution2D {
public:
    Distribution2D(const Float* pdf, int width, int height) noexcept {
        pConditonal.reserve(height);
        for (auto i = 0; i < height; ++i)
            pConditonal.emplace_back(&pdf[width * i], width);
        std::vector<Float> marginalPdf;
        marginalPdf.reserve(height);
        for (auto& distrib : pConditonal) marginalPdf.push_back(distrib.funcIntegral);
        pMarginal = Distribution1D(&marginalPdf[0], height);
    }

    Vector2f sampleContinuous(const Vector2f& sample, Float& pdf) const {
        int w, h;
        Float pdfs[2];
        auto v = pMarginal.sampleContinuous(sample[0], pdfs[0], h);
        auto u = pConditonal[h].sampleContinuous(sample[1], pdfs[1], w);
        pdf = pdfs[0] * pdfs[1];
        return Vector2f(u, v);
    }

    Float pdf(const Vector2f& point) const {
        auto width = pConditonal[0].count();
        auto height = pMarginal.count();
        auto u = (int)(width * point[0]);
        auto v = (int)(height * point[1]);
        return pConditonal[v].pdf[u] * pConditonal[v].funcIntegral / pMarginal.funcIntegral * width * height;
    }

private:
    Distribution1D pMarginal;
    std::vector<Distribution1D> pConditonal;
};

}

#endif
