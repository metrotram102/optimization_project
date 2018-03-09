#include "milp.h"

MixedIntegerLinearProgram::MixedIntegerLinearProgram() : validFlag(false),
                                                         integerFlag(false),
                                                         unsavedChangesFlag(false) {
}

bool MixedIntegerLinearProgram::isValid() {
    return validFlag;
}

bool MixedIntegerLinearProgram::isInteger() {
    return integerFlag;
}

bool MixedIntegerLinearProgram::unsavedChanges() {
    return unsavedChangesFlag;
}

size_t MixedIntegerLinearProgram::solutions_number() {
    return solutions.size();
}

Solution MixedIntegerLinearProgram::get_solution(size_t index) {
    return solutions[index];
}

NEOS_Query MixedIntegerLinearProgram::get_last_query() {
    return last_query;
}

ValueType MixedIntegerLinearProgram::solve(const QString& solver,
                                           const Parameters& params) {
    Solution sol;
    sol.solver = solver;
    sol.params = params;
    //TODO
    sol.value = 0.0;
    sol.vector.assign(objective.size(), 0.0);
    solutions.push_back(sol);
    unsavedChangesFlag = true;
    return 0.0;
}

void MixedIntegerLinearProgram::ExportLP(const QString& file) {
    std::ofstream fout;
    fout.open(file.toStdString().c_str());
    fout << "Maximize\n obj:";
    for (size_t i = 0; i < objective.size(); ++i) {
        fout << " ";
        if (objective[i] < 0.0) {
            fout << "- ";
        } else if (i) {
            fout << "+ ";
        }
        fout << abs(objective[i]) << " x" << i + 1;
    }
    fout << "\nSubject To\n";
    for (size_t i = 0; i < matrix.size(); ++i) {
        fout << " c" << i + 1 << ":";
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            fout << " ";
            if (matrix[i][j] < 0.0) {
                fout << "- ";
            } else if (j) {
                fout << "+ ";
            }
            fout << abs(matrix[i][j]) << " x" << j + 1;
        }
        fout << " <= " << constraints[i] << "\n";
    }
    if (integerFlag) {
        fout << "General\n";
        for (size_t i = 0; i < integer.size(); ++i) {
            if (integer[i]) {
                fout << " x" << i + 1;
            }
        }
        fout << "\n";
    }
    fout << "End\n";
    fout.close();
}

NEOS_Query MixedIntegerLinearProgram::send(const QString& solver,
                                           const Parameters& params,
                                           const QString& email,
                                           const QString& file) {
    ExportLP(file);
    QString number = "empty", password = "empty";
    //TODO
    unsavedChangesFlag = true;
    last_query = {solver, number, password};
    return last_query;
}

void MixedIntegerLinearProgram::clear() {
    validFlag = false;
    integerFlag = false;
    unsavedChangesFlag = false;
    matrix.clear();
    constraints.clear();
    objective.clear();
    integer.clear();
    solutions.clear();
    last_query = {"empty", "empty", "empty"};
}

void MixedIntegerLinearProgram::create(const QString& matrixFile,
                                       const QString& constraintsFile,
                                       const QString& objectiveFile,
                                       const QString& integerFile) {
    clear();
    std::ifstream fin;

    fin.open(matrixFile.toStdString().c_str());
    size_t n, m;
    fin >> n >> m;
    matrix.assign(n, std::vector<ValueType>(m));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            fin >> matrix[i][j];
        }
    }
    fin.close();

    fin.open(constraintsFile.toStdString().c_str());
    fin >> n;
    constraints.assign(n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        fin >> constraints[i];
    }
    fin.close();

    fin.open(objectiveFile.toStdString().c_str());
    fin >> m;
    objective.assign(m, 0.0);
    for (size_t i = 0; i < m; ++i) {
        fin >> objective[i];
    }
    fin.close();

    if (integerFile == QString()) {
        integerFlag = false;
        integer.assign(m, 0);
    } else {
        integerFlag = true;
        fin.open(integerFile.toStdString().c_str());
        fin >> m;
        integer.assign(m, 0);
        for (size_t i = 0; i < m; ++i) {
            fin >> integer[i];
        }
        fin.close();
    }

    if (matrix.size() == 0 ||
        matrix[0].size() == 0 ||
        matrix.size() != constraints.size() ||
        matrix[0].size() != objective.size() ||
        matrix[0].size() != integer.size() ||
        objective.size() != integer.size()) {
        validFlag = false;
    } else {
        validFlag = true;
    }

    unsavedChangesFlag = true;
}

void MixedIntegerLinearProgram::open(const QString& file) {
    clear();
    
    std::ifstream fin;
    fin.open(file.toStdString().c_str());
    
    std::string label;
    size_t n, m;    
    fin >> label >> n >> m; //Size
    
    fin >> label; //Matrix
    matrix.assign(n, std::vector<ValueType>(m));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            fin >> matrix[i][j];
        }
    }
    
    fin >> label; //Constraints
    constraints.assign(n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        fin >> constraints[i];
    }
    
    fin >> label; //Objective
    objective.assign(m, 0.0);
    for (size_t i = 0; i < m; ++i) {
        fin >> objective[i];
    }
    
    fin >> label; //Integer_Variables
    integer.assign(m, 0);
    for (size_t i = 0; i < m; ++i) {
        fin >> integer[i];
        if (integer[i]) {
            integerFlag = true;
        }
    }
    
    size_t solCount;
    fin >> label >> solCount; //Solutions
    solutions.resize(solCount);
    for (size_t i = 0; i < solCount; ++i) {
        std::string solverName;
        ValueType value;
        fin >> label >> solverName >> value;
        solutions[i].solver = QString(solverName.c_str());
        solutions[i].value = value;
        solutions[i].vector.resize(m);
        for (size_t j = 0; j < m; ++j) {
            fin >> solutions[i].vector[j];
        }
    }
    
    std::string solverName, number, password;
    fin >> label >> solverName >> number >> password; //Last_NEOS_Query
    last_query.solver = QString(solverName.c_str());
    last_query.number = QString(number.c_str());
    last_query.password = QString(password.c_str());

    validFlag = true;
}

void MixedIntegerLinearProgram::save(const QString& file) {    
    std::ofstream fout;
    fout.open(file.toStdString().c_str());
    fout << "Size " << matrix.size() << " " << matrix[0].size() << "\n\n";

    fout << "Matrix\n";
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            fout << matrix[i][j] << " ";
        }
        fout << "\n";
    }

    fout << "Constraints\n";
    for (size_t i = 0; i < constraints.size(); ++i) {
        fout << constraints[i] << " ";
    }
    fout << "\n";

    fout << "Objective\n";
    for (size_t i = 0; i < objective.size(); ++i) {
        fout << objective[i] << " ";
    }
    fout << "\n";

    fout << "Integer_Variables\n";
    for (size_t i = 0; i < integer.size(); ++i) {
        fout << integer[i] << " ";
    }
    fout << "\n\n";

    fout << "Solutions " << solutions.size() << "\n";
    for (size_t i = 0; i < solutions.size(); ++i) {
        fout << i + 1 << " " << solutions[i].solver.toStdString() << "\n";
        fout << solutions[i].value << "\n";
        for (size_t j = 0; j < solutions[i].vector.size(); ++j) {
            fout << solutions[i].vector[j] << " ";
        }
        fout << "\n";
    }

    fout << "\nLast_NEOS_Query\n";
    fout << last_query.solver.toStdString() << "\n";
    fout << last_query.number.toStdString() << "\n";
    fout << last_query.password.toStdString() << "\n";
    fout.close();

    unsavedChangesFlag = false;
}
