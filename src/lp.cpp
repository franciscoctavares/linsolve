#include "../include/lp.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

// PRIVATE METHODS

bool LpProblem::isSimplexDone(Matrix cj_minus_zj) {
    for(int i = 0; i < cj_minus_zj.columns(); i++) {
        if(cj_minus_zj.getElement(0, i) > 0) return false;
    }
    return true;
}

unsigned LpProblem::getPivotRow(std::vector<double> simplexAux, std::vector<double> bAux, Matrix ratios) {
    double minValue = M;
    unsigned minIndex = 0;
    unsigned invalid = 0;
    for(int i = 0; i < ratios.rows(); i++) {
        if(simplexAux[i] <= 0) invalid++;
    }
    if(invalid == ratios.rows()) return -1;
    for(int i = 0; i < ratios.rows(); i++) {
        if(ratios.getElement(i, 0) < minValue && ratios.getElement(i, 0) > 0 && ratios.getElement(i, 0) != M) {
            minValue = ratios.getElement(i, 0);
            minIndex = i;
        }
    }
    //std::cout << "minValue = " << ratios.getElement(minIndex, 0) << " and minIndex = " << minIndex << std::endl << std::endl;
    return minIndex;
}

Matrix LpProblem::getBasisIndexes(Matrix extraCj) {
    std::vector<std::vector<int>> restrictionsIndices;
    std::vector<ConstraintType> constraintsTypes;
    for(int i = 0; i < constraints.size(); i++) constraintsTypes.push_back(constraints[i].getType());
    for(int i = 0; i < constraints.size(); i++) restrictionsIndices.push_back({-1, -1});


    unsigned n_slack_surplus_variables = 0;
    unsigned totalExtraVariables = extraCj.columns();
    unsigned currentCoefficient = 0;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraintsTypes[i] == LESS_THAN_OR_EQUAL) {
            restrictionsIndices[i][0] = currentCoefficient;
            currentCoefficient++;
            restrictionsIndices[i][1] = -2; // nas restrições <= não há variáveis artificiais
            n_slack_surplus_variables++;
        }
        else if(constraintsTypes[i] == GREATER_THAN_OR_EQUAL) {
            restrictionsIndices[i][0] = currentCoefficient;
            currentCoefficient++;
            n_slack_surplus_variables++;
        }
        else if(constraintsTypes[i] == EQUAL) {
            restrictionsIndices[i][0] = -2; // nas restrições = não há coeficiente do zero
        }
    }

    std::vector<unsigned> artificialRestrictions;
    for(int i = 0; i < constraintsTypes.size(); i++) {
        if(restrictionsIndices[i][1] == -2) continue;
        else if(restrictionsIndices[i][1] == -1) artificialRestrictions.push_back(i);
    }

    for(int i = n_slack_surplus_variables; i < totalExtraVariables; i++) {
        if(artificialRestrictions.size() > 0) {
            restrictionsIndices[artificialRestrictions[0]][1] = i;
            artificialRestrictions.erase(artificialRestrictions.begin());
        }
    }

    for(int i = 0; i < restrictionsIndices.size(); i++) {
        if(restrictionsIndices[i][0] != -2) restrictionsIndices[i][0] += objectiveFunction.columns();
        if(restrictionsIndices[i][1] != -2) restrictionsIndices[i][1] += objectiveFunction.columns();
        //std::cout << "[" << restrictionsIndices[i][0] << ", " << restrictionsIndices[i][1] << "]" << std::endl;
    }

    std::vector<double> basisIndices;
    for(int i = 0; i < constraintsTypes.size(); i++) {
        if(constraintsTypes[i] == LESS_THAN_OR_EQUAL) basisIndices.push_back(restrictionsIndices[i][0]);
        else if(constraintsTypes[i] == GREATER_THAN_OR_EQUAL) basisIndices.push_back(restrictionsIndices[i][1]);
        else if(constraintsTypes[i] == EQUAL) basisIndices.push_back(restrictionsIndices[i][1]);
    }

    return Matrix(basisIndices, constraintsTypes.size(), 1);
}

Matrix LpProblem::getConstraintsLHS() {
    Matrix aux(constraints[0].getLhs(), 1, constraints[0].getLhs().size());
    for(int i = 1; i < constraints.size(); i++) {
        aux.stackVertical(Matrix(constraints[i].getLhs(), 1, aux.columns()));
    }
    return aux;
}

std::vector<ConstraintType> LpProblem::getConstraintsTypes() {
    std::vector<ConstraintType> aux;
    for(int i = 0; i < constraints.size(); i++) aux.push_back(constraints[i].getType());
    return aux;
}

Matrix LpProblem::getConstraintsRHS() {
    std::vector<double> aux;
    for(int i = 0; i < constraints.size(); i++) aux.push_back(constraints[i].getRhs());
    return Matrix(aux, aux.size(), 1);
}

// PUBLIC METHODS

LpProblem::LpProblem(ProblemType modelType, std::vector<double> newObjectiveFunction, std::vector<Constraint> newConstraints) {
    type = modelType;
    objectiveFunction = Matrix(newObjectiveFunction, 1, newObjectiveFunction.size());
    constraints = newConstraints;
    status = NOT_YET_SOLVED;
}

LpProblem::LpProblem(const LpProblem& problem) {
    type = problem.type;
    objectiveFunction = problem.objectiveFunction;
    constraints = problem.constraints;
    optimalSolution = problem.optimalSolution;
    status = problem.status;
}

bool LpProblem::isConstraintSatisfied(Matrix potentialSolution, int constraintIndex) {
    if(constraintIndex < 0 || constraintIndex >= constraints.size()) {
        std::ostringstream errorMsg;
        errorMsg << "The LP model only has " << constraints.size() << " constraints, but user tried to access constraint with index " << constraintIndex;
        throw std::runtime_error(errorMsg.str());
    }

    //Matrix potentialSolutionAux(potentialSolution, 1, potentialSolution.size());
    Matrix constraintLhs(constraints[constraintIndex].getLhs(), 1, constraints[constraintIndex].getLhs().size());
    ConstraintType restType = constraints[constraintIndex].getType();
    double rhs = constraints[constraintIndex].getRhs();
    double value = potentialSolution.dotProduct(constraintLhs);
    
    if(restType == LESS_THAN_OR_EQUAL) return (value <= rhs) ? true : false;    // <=
    else if(restType == EQUAL) return (value == rhs) ? true : false;            // =
    else return (value >= rhs) ? true : false;                                  // >=
}

bool LpProblem::isSolutionAdmissible(Matrix potentialSolution) {
    for(int i = 0; i < constraints.size(); i++) {
        if(!isConstraintSatisfied(potentialSolution, i)) return false;
    }

    // non negativity
    for(int i = 0; i < potentialSolution.columns(); i++) {
        if(potentialSolution.getElement(0, i) < 0) return false;
    }

    return true;
}

Matrix LpProblem::extraVariablesMatrix() {
    unsigned nVariables = 0;
    unsigned slack_surplus_variables = 0;
    unsigned artificial_variables = 0;

    std::vector<std::vector<double>> pairs;

    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == LESS_THAN_OR_EQUAL) {
            nVariables += 1;
            slack_surplus_variables += 1;
        }
        else if(constraints[i].getType() == EQUAL) {
            nVariables += 1;
            artificial_variables += 1;
        }
        else if(constraints[i].getType() == GREATER_THAN_OR_EQUAL) {
            nVariables += 2;
            slack_surplus_variables += 1;
            artificial_variables += 1;
        }
    }

    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == LESS_THAN_OR_EQUAL) {
            pairs.push_back({double(i), 1.0});
        }
        else if(constraints[i].getType() == EQUAL) {
            continue;
        }
        else if(constraints[i].getType() == GREATER_THAN_OR_EQUAL) {
            pairs.push_back({double(i), -1.0});
        }
    }

    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == EQUAL || constraints[i].getType() == GREATER_THAN_OR_EQUAL) {
            //pairs.push_back({double(slack_surplus_variables - 1 + i), 1.0});
            pairs.push_back({double(i), 1.0});
        }
        else continue;
    }

    Matrix aux = zeros(constraints.size(), nVariables);

    for(int i = 0; i < pairs.size(); i++) {
        aux = aux.setColumn(i, basisVector(constraints.size(), pairs[i][0]) * pairs[i][1]);
    }

    return aux;
}

std::vector<Matrix> LpProblem::initialSimplexTableau() {
    //Matrix simplexTableau = restrictionsLHS;
    std::vector<double> firstRow = constraints[0].getLhs();
    Matrix simplexTableau(firstRow, 1, firstRow.size());
    for(int i = 1; i < constraints.size(); i++) simplexTableau.stackVertical(Matrix(constraints[i].getLhs(), 1, firstRow.size()));
    
    simplexTableau.stackHorizontal(extraVariablesMatrix());
    
    firstRow.clear();
    for(Constraint cons: constraints) firstRow.push_back(cons.getRhs());
    Matrix b(firstRow, firstRow.size(), 1);
    Matrix cj = objectiveFunction;
    
    if(type == MIN) cj = cj * -1;

    std::vector<double> aux;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == LESS_THAN_OR_EQUAL || constraints[i].getType() == GREATER_THAN_OR_EQUAL) aux.push_back(0.0);
    }
    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == EQUAL || constraints[i].getType() == GREATER_THAN_OR_EQUAL) {
            if(type == MIN) aux.push_back(-1 * M);
            else if(type == MAX) aux.push_back(-1 * M);
        }
    }

    Matrix extraCj(aux, 1, aux.size());

    Matrix basisIndicesAux = getBasisIndexes(extraCj);
    cj.stackHorizontal(extraCj);

    std::vector<double> basisThing;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == LESS_THAN_OR_EQUAL) {
            basisThing.push_back(0.0);
        }
        else if(constraints[i].getType() == EQUAL) {
            basisThing.push_back(-1 * M);
        }
        else if(constraints[i].getType() == GREATER_THAN_OR_EQUAL) {
            basisThing.push_back(-1 * M);
        }
    }
    Matrix cb(basisThing, basisThing.size(), 1);

    return {simplexTableau, b, cj, basisIndicesAux, cb};
}

std::vector<std::pair<int, int>> LpProblem::getConstraintsIndexes(Matrix extraCj) {
    std::vector<std::pair<int, int>> constraintsIndexes;
    for(int i = 0; i < constraints.size(); i++) constraintsIndexes.push_back(std::make_pair(-1, -1));


    unsigned n_slack_surplus_variables = 0;
    unsigned totalExtraVariables = extraCj.columns();
    unsigned currentCoefficient = 0;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == LESS_THAN_OR_EQUAL) {
            //constraintsIndexes[i][0] = currentCoefficient;
            constraintsIndexes[i].first = currentCoefficient;
            currentCoefficient++;
            //constraintsIndexes[i][1] = -2; // nas restrições <= não há variáveis artificiais
            constraintsIndexes[i].second = -2; // nas restrições <= não há variáveis artificiais
            n_slack_surplus_variables++;
        }
        else if(constraints[i].getType() == GREATER_THAN_OR_EQUAL) {
            //constraintsIndexes[i][0] = currentCoefficient;
            constraintsIndexes[i].first = currentCoefficient;
            currentCoefficient++;
            n_slack_surplus_variables++;
        }
        else if(constraints[i].getType() == EQUAL) {
            //constraintsIndexes[i][0] = -2; // nas restrições = não há coeficiente do zero
            constraintsIndexes[i].first = -2; // nas restrições = não há coeficiente do zero
        }
    }

    std::vector<unsigned> artificialRestrictions;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraintsIndexes[i].second == -2) continue;
        else if(constraintsIndexes[i].second == -1) artificialRestrictions.push_back(i);
    }

    for(int i = n_slack_surplus_variables; i < totalExtraVariables; i++) {
        if(artificialRestrictions.size() > 0) {
            constraintsIndexes[artificialRestrictions[0]].second = i;
            artificialRestrictions.erase(artificialRestrictions.begin());
        }
    }

    for(int i = 0; i < constraintsIndexes.size(); i++) {
        if(constraintsIndexes[i].first != -2) constraintsIndexes[i].first += objectiveFunction.columns();
        if(constraintsIndexes[i].second != -2) constraintsIndexes[i].second += objectiveFunction.columns();
        //std::cout << "[" << restrictionsIndices[i][0] << ", " << restrictionsIndices[i][1] << "]" << std::endl;
    }
    return constraintsIndexes;
}

void LpProblem::displaySimplexTableau(Matrix tableau, Matrix cb, Matrix basisIndexes, Matrix cj, Matrix b, Matrix zj, Matrix cj_minus_zj) {
    /*
    Matrix extraCj = cj.subMatrix(0, 0, objectiveFunction.columns(), cj.columns() - 1);
    std::vector<std::pair<int, int>> restrictionsIndexes = getConstraintsIndexes(extraCj);

    // compute the maximum width of all numbers to be printed
    size_t maxWidth = 0;


    for(double val: tableau.getElements()) {
        std::string str = std::to_string(val);
        // Trim trailing zeroes for nicer formatting (optional)
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back(); // remove trailing dot if needed
        maxWidth = std::max(maxWidth, str.length());
    }
    for(double val: cj.getElements()) {
        std::string str = std::to_string(val);
        // Trim trailing zeroes for nicer formatting (optional)
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back(); // remove trailing dot if needed
        maxWidth = std::max(maxWidth, str.length());
    }
    for(double val: zj.getElements()) {
        std::string str = std::to_string(val);
        // Trim trailing zeroes for nicer formatting (optional)
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back(); // remove trailing dot if needed
        maxWidth = std::max(maxWidth, str.length());
    }
    for(double val: cj_minus_zj.getElements()) {
        std::string str = std::to_string(val);
        // Trim trailing zeroes for nicer formatting (optional)
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back(); // remove trailing dot if needed
        maxWidth = std::max(maxWidth, str.length());
    }
    double total_z = cb.dotProduct(b);
    std::string total_z_str = std::to_string(total_z);
    total_z_str.erase(total_z_str.find_last_not_of('0') + 1, std::string::npos);
    if (total_z_str.back() == '.') total_z_str.pop_back(); // remove trailing dot if needed
    maxWidth = std::max(maxWidth, total_z_str.length());
    //maxWidth = std::max(maxWidth, std::to_string(cb.dotProduct(b)).length());

    std::vector<std::string> variables;
    for(int i = 0; i < objectiveFunction.columns(); i++) {
        variables.push_back("x" + std::to_string(i + 1));
    }
    for(int k = 0; k < restrictionsIndexes.size(); k++) {
        if(restrictionsIndexes[k].first > 0) variables.push_back("s" + std::to_string(k + 1));
    }
    for(int k = 0; k < restrictionsIndexes.size(); k++) {
        if(restrictionsIndexes[k].second > 0) variables.push_back("a" + std::to_string(k + 1));
    }

    std::ostringstream basis_oss;

    //std::cout << std::endl;



    std::vector<std::string> headers = {"x1", "x2", "x3", "s1", "s2", "s3", "s4", "s5", "s6"};
    std::vector<double> values = {30, 50, 40, 0, 0, 0, 0, 0, 0};

    //std::vector<std::pair<size_t, size_t>> basisWidth = getMaxWidth(headers, cb, "single column");
    std::vector<std::pair<size_t, size_t>> basisWidth = {std::make_pair(10, 10)};
    size_t maxHeaderWidth = basisWidth[0].first;
    size_t maxCoeffsWidth = basisWidth[0].second;

    maxHeaderWidth += 1;  // optional padding
    maxCoeffsWidth += 1;

    std::vector<std::string> basislines;

    size_t line_length_basis = maxHeaderWidth + maxCoeffsWidth + 2 + 2 + 2;
    line_length_basis--;
    
    // Spacing before cj row variables
    for(int i = 0; i < line_length_basis; i++) std::cout << " ";
    std::cout << "|";
    
    // variables row
    for(int i = 0; i < objectiveFunction.columns(); i++) {
        std::cout << std::setw(maxWidth) << "x" << i + 1 << " |";
    }
    for(int j = 0; j < restrictionsIndexes.size(); j++) {
        if(restrictionsIndexes[j].first > 0) {
            std::cout << std::setw(maxWidth) << "s" << j + 1 << " |";
        }
    }
    for(int k = 0; k < restrictionsIndexes.size(); k++) {
        if(restrictionsIndexes[k].second > 0) {

            std::cout << std::setw(maxWidth) << "a" << k + 1 << " |";
        }
    }

    std::cout << std::setw(maxWidth) << " " << "  |";
    
    // Elements of the cj row
    std::cout << std::endl;

    std::cout << "|" << std::setw(maxHeaderWidth) << "xB" << " |" << std::setw(maxCoeffsWidth + 1) << "cB" << " |";
    size_t line_length_basis_aux = line_length_basis - (5 + maxHeaderWidth);
    //for(int i = 0; i < line_length_basis_aux; i++) std::cout << " ";
    for(int i = 0; i < cj.columns(); i++) {
        std::cout << std::setw(maxWidth + 1) << cj.getElement(0, i) << " |";
    }
    std::cout << std::setw(maxWidth + 1) << "b" << " |" << std::endl;

    size_t total_line_length = line_length_basis + ((maxWidth + 3) * cj.columns()) + maxWidth + 1 + 2 + 1;
    for(size_t k = 0; k < total_line_length; k++) std::cout << "-";
    std::cout << std::endl;

    // Display basis variables and its objective function coefficients
    std::vector<std::string> headers_basis = basisHeaders(cj, basisIndexes);
    std::vector<double> basisCoeffs = cb.getElements();
    for(size_t i = 0; i < headers_basis.size(); i++) {
        std::cout << "|" << std::setw(maxHeaderWidth) << headers_basis[i] << " |"
                         << std::setw(maxCoeffsWidth + 1) << basisCoeffs[i] << " |";
        for(size_t j = 0; j < tableau.columns(); j++) {
            std::cout << std::setw(maxWidth + 1) << tableau.getElement(i, j) << " |";
        }

        std::cout << std::setw(maxWidth + 1) << b.getElement(i, 0) << " |";
        std::cout << std::endl;
    }

    for(size_t k = 0; k < total_line_length; k++) std::cout << "-";
    std::cout << std::endl;

    std::cout << std::setw(maxHeaderWidth + maxCoeffsWidth + 1 + 3) << "zj" << " |";

    for(int j = 0; j < zj.columns(); j++) {
        std::cout << std::setw(maxWidth + 1) << zj.getElement(0, j) << " |";
    }

    std::cout << std::setw(maxWidth + 1) << cb.dotProduct(b) << " |" << std::endl;

    std::cout << std::setw(maxHeaderWidth + maxCoeffsWidth + 1 + 3) << "cj - zj" << " |";

    for(int j = 0; j < zj.columns(); j++) {
        std::cout << std::setw(maxWidth + 1) << cj_minus_zj.getElement(0, j) << " |";
    }

    std::cout << std::endl << std::endl;
    */
}

Matrix LpProblem::solveSimplex() {
    Matrix pivots = zeros(1, 2);
    std::vector<Matrix> things = initialSimplexTableau();
    Matrix simplexTableau = things[0];
    Matrix b = things[1];
    Matrix cj = things[2];
    Matrix basisIndices = things[3];
    Matrix cb = things[4];
    
    Matrix zj = zeros(1, simplexTableau.columns());
    Matrix cj_minus_zj = zeros(1, simplexTableau.columns());


    unsigned n_surplus_slack_variables = 0;
    unsigned n_artificial_variables = 0;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == LESS_THAN_OR_EQUAL) n_surplus_slack_variables++;
        else if(constraints[i].getType() == GREATER_THAN_OR_EQUAL) {
            n_surplus_slack_variables++;
            n_artificial_variables++;
        }
        else if(constraints[i].getType() == EQUAL) n_artificial_variables++;
    }

    for(int i = 0; i < simplexTableau.columns(); i++) {
        zj.setElement(0, i, cb.dotProduct(simplexTableau.getColumn(i)));
    }
    cj_minus_zj = cj - zj;

    //displaySimplexTableau(simplexTableau, cb, basisIndices, cj, b, zj, cj_minus_zj);

    unsigned iterations = 0;
    while(!isSimplexDone(cj_minus_zj)) {

        pivots.setElement(0, 1, cj_minus_zj.maxValueIndex());
        Matrix ratios = zeros(constraints.size(), 1);

        ratios = b.pointDivision(simplexTableau.getColumn(pivots.getElement(0, 1)));
        //ratios.displayMatrix();
        //std::cout << std::endl;
        std::vector<double> simplexAux, bAux;
        for(int i = 0; i < ratios.rows(); i++) {
            simplexAux.push_back(simplexTableau.getElement(i, pivots.getElement(0, 1)));
            bAux.push_back(b.getElement(i, 0));
        }

        unsigned pivotRow = getPivotRow(simplexAux, bAux, ratios);
        // unbounded problem
        if(pivotRow == -1) {
            status = UNBOUNDED;
            optimalSolution = Matrix({INFINITY}, 1, 1);
            return Matrix({INFINITY}, 1, 1);
        }
        pivots.setElement(0, 0, pivotRow);

        unsigned oldBasis = pivots.getElement(0, 0);
        unsigned newBasis = pivots.getElement(0, 1);

        if(cb.getElement(oldBasis, 0) == M || cb.getElement(oldBasis, 0) == -1 * M) {
            unsigned artificial_index = basisIndices.getElement(oldBasis, 0);
            //std::cout << "art_index = " << artificial_index << std::endl;

            simplexTableau.removeColumn(artificial_index);
            cj.removeColumn(artificial_index);
            zj.removeColumn(artificial_index);
            cj_minus_zj.removeColumn(artificial_index);

            //if(newBasis > artificial_index) newBasis--;
            
            if(newBasis >= artificial_index) {
                //std::cout << "New basis out of bounds" << std::endl;
                newBasis -= 1;
            }

            for(int i = 0; i < basisIndices.rows(); i++) {
                if(basisIndices.getElement(i, 0) > artificial_index) basisIndices.setElement(i, 0, basisIndices.getElement(i, 0) - 1.0);
            }
        }

        basisIndices.setElement(oldBasis, 0, newBasis);
        cb.setElement(oldBasis, 0, cj.getElement(0, newBasis));

        Matrix newRow = Matrix(simplexTableau.getRow(oldBasis), 1, simplexTableau.columns()) * (1 / simplexTableau.getElement(oldBasis, newBasis));
        b.setElement(oldBasis, 0, b.getElement(oldBasis, 0) / simplexTableau.getElement(oldBasis, newBasis));

        simplexTableau = simplexTableau.setRow(oldBasis, newRow);

        for(int i = 0; i < simplexTableau.rows(); i++) {
            if(i == oldBasis) continue;
            else {
                double factor = simplexTableau.getElement(i, newBasis);
                simplexTableau.rowOperation(oldBasis, i, -1 * factor);
                b.rowOperation(oldBasis, i, -1 * factor);
            }
        }

        for(int i = 0; i < simplexTableau.columns(); i++) {
            zj.setElement(0, i, cb.dotProduct(simplexTableau.getColumn(i)));
        }
        cj_minus_zj = cj - zj;

        iterations++;

    }

    //displaySimplexTableau(simplexTableau, cb, basisIndices, cj, b, zj, cj_minus_zj);

    Matrix solution = zeros(1, objectiveFunction.columns());

    for(int k = 0; k < basisIndices.rows(); k++) {
        if(basisIndices.getElement(k, 0) < objectiveFunction.columns()) solution.setElement(0, basisIndices.getElement(k, 0), b.getElement(k, 0));
        else if(basisIndices.getElement(k, 0) >= objectiveFunction.columns() + n_surplus_slack_variables && b.getElement(k, 0) > 0) {
            // infeasible problem
            //std::cout << "Hello there" << std::endl;
            status = INFEASIBLE;
            solution = Matrix({0}, 1, 1);
            break;
        }
    }


    optimalSolution = solution;
    return solution;
}

void LpProblem::displayProblem() {
    // Objective function
    if(type == MAX) std::cout << "max z: ";
    else if(type == MIN) std::cout << "min z: ";

    for(int i = 0; i < objectiveFunction.columns(); i++) {
        if(objectiveFunction.getElement(0, i) < 0) std::cout << "- ";
        else if(objectiveFunction.getElement(0, i) > 0 && i > 0) std::cout << "+ ";

        if(objectiveFunction.getElement(0, i) != 1 && floor(objectiveFunction.getElement(0, i)) != objectiveFunction.getElement(0, i)) 
            std::cout << std::setprecision(3) << std::fixed << objectiveFunction.getElement(0, i);
        else if(objectiveFunction.getElement(0, i) != 1 && floor(objectiveFunction.getElement(0, i) != 1) == objectiveFunction.getElement(0, i) != 1)
            std::cout << unsigned(objectiveFunction.getElement(0, i));
        std::cout << "x" << i + 1;

        if(i < objectiveFunction.columns() - 1) std::cout << " ";
    }
    std::cout << std::endl << std::endl;;

    // Constraints
    std::cout << "subject to:" << std::endl;
    for(int i = 0; i < constraints.size(); i++) {
        bool hasWritten = false;
        std::vector<double> lhs = constraints[i].getLhs();
        ConstraintType currentType = constraints[i].getType();
        double rhs = constraints[i].getRhs();
        for(int j = 0; j < lhs.size(); j++) {
            if(lhs[j] != 0) {
                if(lhs[j] < 0) std::cout << "- ";
                else if(lhs[j] > 0 && j > 0 && hasWritten) std::cout << "+ ";

                if(lhs[j] != 1 && floor(lhs[j]) != lhs[j]) std::cout << std::setprecision(3) << std::fixed << fabs(lhs[j]);
                else if(lhs[j] != 1 && floor(lhs[j]) == lhs[j]) std::cout << unsigned(lhs[j]);

                std::cout << "x" << j + 1;
                hasWritten = true;
            }

            if(j < lhs.size() - 1) std::cout << " ";
        }

        if(currentType == LESS_THAN_OR_EQUAL) std::cout << " <= ";
        else if(currentType == GREATER_THAN_OR_EQUAL) std::cout << " >= ";
        else if(currentType == EQUAL) std::cout << " = ";

        if(rhs < 0) std::cout << "-";
        if(floor(rhs) == rhs) std::cout << unsigned(rhs);
        else std::cout << std::setprecision(3) << std::fixed << fabs(rhs);
        std::cout << std::endl;
    }
    std::cout << std::endl;

    if(status == NOT_YET_SOLVED) std::cout << "The problem was not solved yet" << std::endl;
    else if(status == INFEASIBLE) std::cout << "The problem is infeasible" << std::endl;
    else if(status == UNBOUNDED) std::cout << "The problem is unbounded" << std::endl;
    else {
        std::cout << "The optimal solution is (";
        for(int i = 0; i < optimalSolution.columns(); i++) {
            std::cout << "x" << i + 1;
            if(i < optimalSolution.columns() - 1) std::cout << ", ";
        }
        std::cout << ") = (";
        for(int i = 0; i < optimalSolution.columns(); i++) {
            if(floor(optimalSolution.getElement(0, i)) == optimalSolution.getElement(0, i)) std::cout << unsigned(optimalSolution.getElement(0, i));
            else std::cout << std::setprecision(3) << std::fixed << optimalSolution.getElement(0, i);
            if(i < optimalSolution.columns() - 1) std::cout << ", ";
        }
        std::cout << "), and Z = ";
        if(floor(optimalSolution.dotProduct(objectiveFunction)) == optimalSolution.dotProduct(objectiveFunction)) 
            std::cout << unsigned(optimalSolution.dotProduct(objectiveFunction)) << std::endl;
        else std::cout << std::setprecision(3) << std::fixed << optimalSolution.dotProduct(objectiveFunction) << std::endl;
    }
}

void LpProblem::addConstraint(Constraint newConstraint) {
    constraints.push_back(newConstraint);
}

void LpProblem::removeConstraint(int constraintIndex) {
    constraints.erase(constraints.begin() + constraintIndex);
}

bool LpProblem::isProblemFeasible(Matrix candidateSolution) {
    if(candidateSolution.rows() == 1 && candidateSolution.columns() == 1 && candidateSolution.getElement(0, 0) == 0) return false;
    else return true;
}

bool LpProblem::isProblemBounded(Matrix candidateSolution) {
    if(candidateSolution.rows() == 1 && candidateSolution.columns() == 1 && candidateSolution.getElement(0, 0) == INFINITY) return false;
    else return true;
}

Matrix LpProblem::getOptimalSolution() {
    return optimalSolution;
}

void LpProblem::setOptimalSolution(Matrix newOptimalSolution) {
    optimalSolution = newOptimalSolution;
}

ProblemType LpProblem::getType() {
    return type;
}

Matrix LpProblem::getObjectiveFunction() {
    return objectiveFunction;
}

bool LpProblem::canProblemBeSimplified(SimplifiedConstraintsHelper* helper) {
    
    checkForRepeatedConstraints(helper);

    // Checks helper for repeated indexes
    for(int i = 0; i < helper->repeatedConstraints.size(); i++) {
        for(int j = 0; j < helper->repeatedConstraints.size(); j++) {
            if(i == j) continue;
            if(helper->repeatedConstraints[i] == helper->repeatedConstraints[j] && i < j) {
                helper->repeatedConstraints.erase(helper->repeatedConstraints.begin() + j);
            }
        }
    }

    // Check if any variable's value can be fixed
    canVariablesBeFixed(helper);
    std::vector<uint> repeatedIndexes;
    for(uint i = 0; i < helper->constraintsToRemove.size(); i++) {
        for(uint j = 0; j < helper->constraintsToRemove.size(); j++) {
            if(helper->constraintsToRemove[i] == helper->constraintsToRemove[j] && j > i) {
                repeatedIndexes.push_back(j);
            }
        }
    }

    std::sort(repeatedIndexes.begin(), repeatedIndexes.end(), std::greater<unsigned>());
    for(uint i: repeatedIndexes) {
        helper->constraintsToRemove.erase(helper->constraintsToRemove.begin() + i);
    }
    std::sort(helper->constraintsToRemove.begin(), helper->constraintsToRemove.end(), std::greater<unsigned>());

    removeRepeatedFixedVariablesPairs(helper);

    if(helper->constraintsToRemove.size() == 0 && helper->fixedVariables.size() == 0) return false;

    return true;
}

bool LpProblem::simplifyProblem(SimplifiedConstraintsHelper* helper) {
    removeConstraints(helper);

    // para variaveis fixadas, eliminar essas variaveis e usar o modelo "oldVariablesToNewVariables"

    checkForRepeatedConstraints(helper);

    canVariablesBeFixed(helper);

    newVarsToOldVars(helper);

    std::vector<uint> repeatedIndexes;
    for(uint i = 0; i < helper->constraintsToRemove.size(); i++) {
        for(uint j = 0; j < helper->constraintsToRemove.size(); j++) {
            if(helper->constraintsToRemove[i] == helper->constraintsToRemove[j] && j > i) {
                repeatedIndexes.push_back(j);
            }
        }
    }

    std::sort(repeatedIndexes.begin(), repeatedIndexes.end(), std::greater<unsigned>());
    for(uint i: repeatedIndexes) {
        helper->constraintsToRemove.erase(helper->constraintsToRemove.begin() + i);
    }
    std::sort(helper->constraintsToRemove.begin(), helper->constraintsToRemove.end(), std::greater<unsigned>());

    removeFixedVariables(helper);

    return true;
}

void LpProblem::simplifiedProblemSolution(SimplifiedConstraintsHelper* helper,  Matrix simplifiedSolution) {

    Matrix unboundedSol = Matrix({INFINITY}, 1, 1);
    Matrix infeasibleSol = Matrix({0}, 1, 1);

    if(simplifiedSolution == infeasibleSol) {
        status = INFEASIBLE;
        optimalSolution = infeasibleSol;
        return;
    }
    else if(simplifiedSolution == unboundedSol) {
        status = UNBOUNDED;
        optimalSolution = unboundedSol;
        return;
    }

    Matrix actualSolution = zeros(1, simplifiedSolution.columns() + helper->fixedVariables.size());

    for(std::pair pairsOfVars : helper->pairsOfVars) {
        actualSolution.setElement(0, pairsOfVars.second, simplifiedSolution.getElement(0, pairsOfVars.first));
    }

    for(std::pair fixedVars : helper->fixedVariables) {
        actualSolution.setElement(0, fixedVars.first, fixedVars.second);
    }

    optimalSolution = actualSolution;
}

void LpProblem::canVariablesBeFixed(SimplifiedConstraintsHelper* helper) {
    std::vector<std::tuple<std::vector<unsigned>, unsigned, double>> fixedVariables;
    unsigned n_vars = objectiveFunction.columns();

    // Check for constraints of the form xi = k, where i = 1,...,n and k is a real number
    for(uint i = 0; i < constraints.size(); i++) {
        int fix_var = Matrix(constraints[i].getLhs(), 1, n_vars).isBasisVector();
        std::vector<unsigned> constraintsToRemove = {i};
        if(fix_var != -1 && constraints[i].getType() == EQUAL) {
            helper->constraintsToRemove.push_back(i);
            helper->fixedVariables.push_back(std::make_pair(fix_var, constraints[i].getRhs()));
        }
    }

    
    std::vector<unsigned> basisConstraints;
    for(int i = 0; i < constraints.size(); i++) {
        int basis = Matrix(constraints[i].getLhs(), 1, n_vars).isBasisVector();
        if(basis != -1 && constraints[i].getType() != EQUAL) basisConstraints.push_back(i);
    }
    
    // Check for pairs of constraints like xi <= k and xi >= k, where i = 1,...,n and k is a real number
    for(uint i = 0; i < basisConstraints.size(); i++) {
        uint constraint_i = basisConstraints[i];
        for(uint j = 0; j < basisConstraints.size(); j++) {
            uint constraint_j = basisConstraints[j];
            if(constraint_i == constraint_j || constraint_i > constraint_j) continue;
            else {
                if(constraints[constraint_i].getLhs() == constraints[constraint_j].getLhs() &&
                   constraints[constraint_i].getType() != constraints[constraint_j].getType() &&
                   constraints[constraint_i].getRhs() == constraints[constraint_j].getRhs()) {
                        std::vector<unsigned> constraintsToRemove = {constraint_i, constraint_j};
                        int fixedVariableIndex = Matrix(constraints[constraint_i].getLhs(), 1, n_vars).isBasisVector();
                        helper->fixedVariables.push_back(std::make_pair(fixedVariableIndex, constraints[constraint_i].getRhs()));
                        helper->constraintsToRemove.push_back(constraint_i);
                        helper->constraintsToRemove.push_back(constraint_j);
                }
            }
        }

    }

    // Check for constraints of the form xi <= 0
    for(uint i = 0; i < basisConstraints.size(); i++) {
        uint currentConstraint = basisConstraints[i];
        if(constraints[currentConstraint].getType() == LESS_THAN_OR_EQUAL && constraints[currentConstraint].getRhs() == 0) {
            std::vector<unsigned> constraintsToRemove = {currentConstraint};
            int fixedVariableIndex = Matrix(constraints[currentConstraint].getLhs(), 1, n_vars).isBasisVector();
            helper->fixedVariables.push_back(std::make_pair(fixedVariableIndex, constraints[currentConstraint].getRhs()));
            helper->constraintsToRemove.push_back(currentConstraint);
        }
    }

    // remove repeated fixed variables

    std::vector<uint> repeatedIndexes;

    for(uint i = 0; i < helper->fixedVariables.size(); i++) {
        for(uint j = 0; j < helper->fixedVariables.size(); j++) {
            if(helper->fixedVariables[i].first == helper->fixedVariables[j].first && helper->fixedVariables[i].second == helper->fixedVariables[i].second && j > i) {
                repeatedIndexes.push_back(j);
            }
        }
    }

    std::sort(repeatedIndexes.begin(), repeatedIndexes.end(), std::greater<unsigned>());
    for(uint i : repeatedIndexes) {
        helper->fixedVariables.erase(helper->fixedVariables.begin() + i);
    }

    std::sort(helper->fixedVariables.begin(), helper->fixedVariables.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
}

void LpProblem::checkForRepeatedConstraints(SimplifiedConstraintsHelper* helper) {
    // Check for repeated constraints
    for(uint i = 0; i < constraints.size(); i++) {
        for(uint j = 0; j < constraints.size(); j++) {
            if(i == j) continue;
            if(constraints[i] == constraints[j] && i < j) {
                //std::cout << "Constraint " << i << " is equal to constraint " << j << std::endl;
                helper->constraintsToRemove.push_back(j);
            }
        }
    }
}

void LpProblem::removeRepeatedFixedVariablesPairs(SimplifiedConstraintsHelper* helper) {
    std::vector<uint> repeatedIndexes;
    for(uint i = 0; i < helper->fixedVariables.size(); i++) {
        for(uint j = 0; j < helper->fixedVariables.size(); j++) {
            if(helper->fixedVariables[i] == helper->fixedVariables[j] && j > i) {
                repeatedIndexes.push_back(j);
            }
        }
    }

    std::sort(repeatedIndexes.begin(), repeatedIndexes.end(), std::greater<unsigned>());

    for(uint k : repeatedIndexes) {
        helper->fixedVariables.erase(helper->fixedVariables.begin() + k);
    }
}

void LpProblem::newVarsToOldVars(SimplifiedConstraintsHelper* helper) {
    std::vector<uint> fixedVars;
    for(std::pair k : helper->fixedVariables) {
        fixedVars.push_back(k.first);
    }

    std::vector<std::pair<uint, uint>> pairsOfVars;
    
    std::vector<uint> freeVars;
    for(uint i = 0; i < objectiveFunction.columns(); i++) {
        if(std::find(fixedVars.begin(), fixedVars.end(), i) == fixedVars.end()) {
            freeVars.push_back(i);
        }
    }

    for(uint i = 0; i < freeVars.size(); i++) {
        pairsOfVars.push_back(std::make_pair(i, freeVars[i]));
    }

    helper->pairsOfVars = pairsOfVars;
}

void LpProblem::removeConstraints(SimplifiedConstraintsHelper* helper) {
    for(uint i : helper->constraintsToRemove) {
        removeConstraint(i);
    }
}

void LpProblem::removeOneFixedVariable(uint varIndex, double fixedVarValue) {
    objectiveFunction.removeColumn(varIndex);

    for(uint i = 0; i < constraints.size(); i++) {
        constraints[i].removeFixedVariable(varIndex, fixedVarValue);
    }
}

void LpProblem::removeFixedVariables(SimplifiedConstraintsHelper* helper) {
    for(std::pair fixedVars : helper->fixedVariables) {
        removeOneFixedVariable(fixedVars.first, fixedVars.second);
    }
}

void LpProblem::solveProblem() {
    SimplifiedConstraintsHelper helper;
    if(canProblemBeSimplified(&helper)) {
        if(helper.fixedVariables.size() == objectiveFunction.columns()) {
	        optimalSolution = zeros(1, objectiveFunction.columns());
            for(std::pair<uint, double> fixedVars: helper.fixedVariables) {
                optimalSolution.setElement(0, fixedVars.first, fixedVars.second);
            }
        }
        else {
            LpProblem auxProblem(type, objectiveFunction.getElements(), constraints);
            auxProblem.simplifyProblem(&helper);
            auxProblem.solveSimplex();
            simplifiedProblemSolution(&helper, auxProblem.getOptimalSolution());
        }
    }
    else {
        solveSimplex();
    }

    isOptimalSolutionWhole();
}

bool LpProblem::operator==(ProblemStatus statusToCheck) {
    return (status == statusToCheck) ? true : false;
}

bool LpProblem::operator!=(ProblemStatus statusToCheck) {
    return (status != statusToCheck) ? true : false;
}

void LpProblem::setSolutionType() {
    //std::cout << optimalSolution.columns() << " variables" << std::endl;
    for(uint i = 0; i < optimalSolution.columns(); i++) {
        //std::cout << optimalSolution.getElement(0, i) << " ";
        if(floor(optimalSolution.getElement(0, i)) != optimalSolution.getElement(0, i)) {
            status = CONTINUOUS_SOLUTION;
            return;
        }
    }
    //std::cout << std::endl;
    status = WHOLE_SOLUTION;
}

bool LpProblem::isOptimalSolutionWhole() {
    Matrix infeasibleSol({0}, 1, 1);
    Matrix unboundedSol({INFINITY}, 1, 1);

    if(optimalSolution == infeasibleSol) {
        status = INFEASIBLE;
        return false;
    }
    else if(optimalSolution == unboundedSol) {
        status = UNBOUNDED;
        return false;
    }

    for(uint i = 0; i < optimalSolution.columns(); i++) {
        std::pair<bool, double> currentPair = isDoubleAnInteger(optimalSolution.getElement(0, i), 1e-10);
        if(!currentPair.first) {
            status = CONTINUOUS_SOLUTION;
            return false;
        }
        else optimalSolution.setElement(0, i, currentPair.second);
    }
    status = WHOLE_SOLUTION;
    return true;
}

std::vector<Constraint> LpProblem::getConstraints() {
    return constraints;
}

ProblemStatus LpProblem::getStatus() {
    return status;
}

// Non LpProblem class functions

std::pair<bool, double> isDoubleAnInteger(double number, double epsilon) {
    if(std::abs(number - std::round(number)) < epsilon) {
        return std::make_pair(true, std::round(number));
    }
    else return std::make_pair(false, number);
}