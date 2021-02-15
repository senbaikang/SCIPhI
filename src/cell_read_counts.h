//
// Created by Senbai Kang on 14.02.21.
//

#ifndef SCIPHI_CELL_READ_COUNTS_H
#define SCIPHI_CELL_READ_COUNTS_H

struct SignificantAltNuc {
  unsigned sigAltNuc{};
  double mean{};
  double pValue{};

public:
  SignificantAltNuc() = default;

  SignificantAltNuc(
      unsigned sigAltNuc,
      double mean,
      double pValue):
    sigAltNuc{sigAltNuc},
    mean{mean},
    pValue{pValue} {}

};

inline bool compareSigAltNucs(
    const SignificantAltNuc & v1,
    const SignificantAltNuc & v2) {
  if (v1.mean != v2.mean) {
    return v1.mean > v2.mean;
  }

  if (v1.pValue != v2.pValue) {
    return v1.pValue > v2.pValue;
  }

  return v1.sigAltNuc < v2.sigAltNuc;
}

struct SignificantAltNucs {
  std::vector<SignificantAltNuc> sigAltNucs{};
  std::vector<unsigned> orderedSigAltNucs{};

public:
  SignificantAltNucs() {
    this->sigAltNucs.resize(0);
    this->orderedSigAltNucs.resize(0);
  }

  void addSigAltNuc(const SignificantAltNuc & v) {
    this->sigAltNucs.resize(this->sigAltNucs.size() + 1, v);
  }

  int getAltNucOrder(unsigned v) const {
    for (int i = 0; i < this->orderedSigAltNucs.size(); i++) {
      if (v == this->orderedSigAltNucs[i]) {
        return i;
      }
    }

    return -1;
  }

  void getOrderedSigAltNucs() {
    std::sort(this->sigAltNucs.begin(), this->sigAltNucs.end(), compareSigAltNucs);

    this->orderedSigAltNucs.resize(this->sigAltNucs.size());
    for (unsigned i = 0; i < this->sigAltNucs.size(); i++) {
      this->orderedSigAltNucs[i] = this->sigAltNucs[i].sigAltNuc;
    }
  }

  void resetSigAltNucs() {
    this->sigAltNucs.clear();
    this->orderedSigAltNucs.clear();

    this->sigAltNucs.resize(0);
    this->orderedSigAltNucs.resize(0);
  }

  template <class type>
  std::vector<type> convertAltNucsType(type (*func)(unsigned)) {
    std::vector<type> results(this->orderedSigAltNucs.size());

    for (unsigned i = 0; i < this->orderedSigAltNucs.size(); i++) {
      results[i] = (*func)(this->orderedSigAltNucs[i]);
    }

    return results;
  }

};

struct AltNucReadCount {
  unsigned altNuc{};
  char altNucChar{};
  unsigned readCount{};
  SignificantAltNucs sigAltNucs{};

public:
  AltNucReadCount() = default;

  AltNucReadCount(
      unsigned altNuc,
      char aluNucChar,
      unsigned readCount,
      const SignificantAltNucs & sigAltNucs
      ) {
    this->altNuc = altNuc;
    this->altNucChar = aluNucChar;
    this->readCount = readCount;
    this->sigAltNucs = sigAltNucs;
  }

  bool operator > (const AltNucReadCount & v) const {
    if (this->readCount != v.readCount) {
      return this->readCount > v.readCount;
    }

    int ord = this->sigAltNucs.getAltNucOrder(this->altNuc);
    int vOrd = v.sigAltNucs.getAltNucOrder(v.altNuc);
    if (ord != vOrd) {
      if (ord == -1 || vOrd == -1) {
        return ord > vOrd;
      } else {
        return ord < vOrd;
      }
    }

    return this->altNuc < v.altNuc;
  }

};

struct CellReadCounts {
  std::array<AltNucReadCount, 3> altNucReadCounts{};
  unsigned cov{};

public:
  CellReadCounts() = default;

  void setMembers(
      const AltNucReadCount & v1,
      const AltNucReadCount & v2,
      const AltNucReadCount & v3,
      unsigned cov
      ) {
    this->altNucReadCounts[0] = v1;
    this->altNucReadCounts[1] = v2;
    this->altNucReadCounts[2] = v3;
    this->cov = cov;
  }

  void sortAltNucReadCounts() {
    std::sort(this->altNucReadCounts.begin(), this->altNucReadCounts.end(), std::greater<AltNucReadCount>());
  }

};

std::ostream & operator<<(std::ostream & out, const CellReadCounts & v)  {
  out << v.altNucReadCounts[0].altNucChar << ","
      << v.altNucReadCounts[1].altNucChar << ","
      << v.altNucReadCounts[2].altNucChar << ";";

  out << v.altNucReadCounts[0].readCount << ","
      << v.altNucReadCounts[1].readCount << ","
      << v.altNucReadCounts[2].readCount << ","
      << v.cov;

  return out;
}

#endif // SCIPHI_CELL_READ_COUNTS_H
