#ifndef MILP_H
#define MILP_H

#include <cmath>
#include <fstream>
#include <QString>
#include <vector>

typedef double ValueType;

struct NEOS_Query {
    QString solver;
    QString number;
    QString password;
};

struct Parameters {
    int p1, p2, p3;
};

struct Solution {
    ValueType value;
    std::vector<ValueType> vector;
    QString solver;
    Parameters params;
};

class MixedIntegerLinearProgram {
public:
    MixedIntegerLinearProgram();

    bool isValid();
    bool isInteger();
    bool unsavedChanges();

    size_t solutions_number();
    Solution get_solution(size_t);
    NEOS_Query get_last_query();

    void ExportLP(const QString&);
    ValueType solve(const QString&, const Parameters&);
    NEOS_Query send(const QString&, const Parameters&,
                    const QString&, const QString&);

    void clear();
    void create(const QString&, const QString&,
                const QString&, const QString&);
    void open(const QString&);
    void save(const QString&);
    
private:
    bool validFlag;
    bool integerFlag;
    bool unsavedChangesFlag;

    std::vector<std::vector<ValueType>> matrix;
    std::vector<ValueType> constraints;
    std::vector<ValueType> objective;
    std::vector<int> integer;

    std::vector<Solution> solutions;
    NEOS_Query last_query;
};

#endif
