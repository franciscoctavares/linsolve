#include "lp.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

// PRIVATE METHODS

bool LpProblem::isSimplexDone(Matrix& cj_minus_zj) {
    for(int i = 0; i < cj_minus_zj.getNColumns(); i++) {
        if(cj_minus_zj.getElement(0, i) > 0) return false;
    }
    return true;
}

unsigned LpProblem::getPivotRow(Matrix& simplexAux, Matrix& bAux, Matrix& ratios) {
    double minValue = M;
    unsigned minIndex = 0;
    unsigned invalid = 0;
    double ratioElement;

    for(int i = 0; i < ratios.getNRows(); i++) {
        if(simplexAux.getElement(i, 0) <= 0) invalid++;
    }
    if(invalid == ratios.getNRows()) return -1;

    for(int i = 0; i < ratios.getNRows(); i++) {
        ratioElement = ratios.getElement(i, 0);
        if(ratioElement < minValue && ratioElement > 0 && ratioElement != M) {
            minValue = ratioElement;
            minIndex = i;
        }
    }

    return minIndex;
}

Matrix LpProblem::getBasisIndexes(Matrix& extraCj) {
    std::vector<std::pair<int, int>> restrictionsIndices(constraints.size(), {-1, -1});
    std::vector<ConstraintType> constraintsTypes;

    for(Constraint& currentConstraint : constraints) {
        constraintsTypes.push_back(currentConstraint.getType());
    }


    unsigned n_slack_surplus_variables = 0;
    unsigned totalExtraVariables = extraCj.getNColumns();
    unsigned currentCoefficient = 0;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraintsTypes[i] == LESS_THAN_OR_EQUAL) {
            restrictionsIndices[i].first = currentCoefficient;
            currentCoefficient++;
            restrictionsIndices[i].second = -2; // nas restrições <= não há variáveis artificiais
            n_slack_surplus_variables++;
        }
        else if(constraintsTypes[i] == GREATER_THAN_OR_EQUAL) {
            restrictionsIndices[i].first = currentCoefficient;
            currentCoefficient++;
            n_slack_surplus_variables++;
        }
        else if(constraintsTypes[i] == EQUAL) {
            restrictionsIndices[i].first = -2; // nas restrições = não há coeficiente do zero
        }
    }



    std::vector<unsigned> artificialRestrictions;
    for(int i = 0; i < constraintsTypes.size(); i++) {
        if(restrictionsIndices[i].second == -2) continue;
        else if(restrictionsIndices[i].second == -1) artificialRestrictions.push_back(i);
    }

    for(int i = n_slack_surplus_variables; i < totalExtraVariables; i++) {
        if(artificialRestrictions.size() > 0) {
            restrictionsIndices[artificialRestrictions[0]].second = i;
            artificialRestrictions.erase(artificialRestrictions.begin());
        }
    }

    for(std::pair<int, int>& currentPair : restrictionsIndices) {
        if(currentPair.first != -2) currentPair.first += objectiveFunction.getNColumns();
        if(currentPair.second != -2) currentPair.second += objectiveFunction.getNColumns();
    }

    std::vector<double> basisIndices;
    for(int i = 0; i < constraintsTypes.size(); i++) {
        if(constraintsTypes[i] == LESS_THAN_OR_EQUAL) basisIndices.push_back(restrictionsIndices[i].first);
        else if(constraintsTypes[i] == GREATER_THAN_OR_EQUAL) basisIndices.push_back(restrictionsIndices[i].second);
        else if(constraintsTypes[i] == EQUAL) basisIndices.push_back(restrictionsIndices[i].second);
    }
    
    return Matrix(basisIndices, constraintsTypes.size(), 1);
}

Matrix LpProblem::getConstraintsLHS() {
    Matrix aux(constraints[0].getLhs(), 1, constraints[0].getLhs().size());
    for(int i = 1; i < constraints.size(); i++) {
        Matrix currentRow = Matrix(constraints[i].getLhs(), 1, aux.getNColumns());
        aux.stackVertical(currentRow);
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

bool LpProblem::isConstraintSatisfied(Matrix potentialSolution, int constraintIndex) {
    if(constraintIndex < 0 || constraintIndex >= constraints.size()) {
        std::ostringstream errorMsg;
        errorMsg << "The LP model only has " << constraints.size() << " constraints, but user tried to access constraint with index " << constraintIndex;
        throw std::invalid_argument(errorMsg.str());
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
    for(int i = 0; i < potentialSolution.getNColumns(); i++) {
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
        Matrix currentBasisVector = basisVector(constraints.size(), pairs[i][0]) * pairs[i][1];
        aux.setColumn(i, currentBasisVector);
    }

    return aux;
}

std::vector<Matrix> LpProblem::initialSimplexTableau() {
    std::vector<double> firstRow = constraints[0].getLhs();
    uint num_variables = constraints[0].getLhs().size();
    Matrix simplexTableau(firstRow, 1, num_variables);
    
    for(int i = 1; i < constraints.size(); i++) {
        Matrix currentLhs(constraints[i].getLhs(), 1, num_variables);
        simplexTableau.stackVertical(currentLhs);
    }

    Matrix extraVars = extraVariablesMatrix();    
    simplexTableau.stackHorizontal(extraVars);
    
    firstRow.clear();
    for(Constraint& currentConstraint : constraints) firstRow.push_back(currentConstraint.getRhs());
    Matrix b(firstRow, firstRow.size(), 1);
    
    Matrix cj = objectiveFunction;
    if(type == MIN) cj *= -1;

    std::vector<double> aux;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() != EQUAL) aux.push_back(0.0);
    }

    std::vector<double> basisThing;
    for(Constraint& currentConstraint : constraints) {        
        if(currentConstraint.getType() == LESS_THAN_OR_EQUAL) {
            basisThing.push_back(0.0);
        }
        else {
            aux.push_back(-1 * M);
            basisThing.push_back(-1 * M);   
        }        
    }

    Matrix extraCj(aux, 1, aux.size());
    Matrix basisIndicesAux = getBasisIndexes(extraCj);
    cj.stackHorizontal(extraCj);
    Matrix cb(basisThing, basisThing.size(), 1);

    return {simplexTableau, b, cj, basisIndicesAux, cb};
}

std::vector<std::pair<int, int>> LpProblem::getConstraintsIndexes(Matrix extraCj) {
    std::vector<std::pair<int, int>> constraintsIndexes;
    for(int i = 0; i < constraints.size(); i++) constraintsIndexes.push_back(std::make_pair(-1, -1));


    unsigned n_slack_surplus_variables = 0;
    unsigned totalExtraVariables = extraCj.getNColumns();
    unsigned currentCoefficient = 0;
    for(int i = 0; i < constraints.size(); i++) {
        if(constraints[i].getType() == LESS_THAN_OR_EQUAL) {
            constraintsIndexes[i].first = currentCoefficient;
            currentCoefficient++;
            constraintsIndexes[i].second = -2; // nas restrições <= não há variáveis artificiais
            n_slack_surplus_variables++;
        }
        else if(constraints[i].getType() == GREATER_THAN_OR_EQUAL) {
            constraintsIndexes[i].first = currentCoefficient;
            currentCoefficient++;
            n_slack_surplus_variables++;
        }
        else if(constraints[i].getType() == EQUAL) {
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
        if(constraintsIndexes[i].first != -2) constraintsIndexes[i].first += objectiveFunction.getNColumns();
        if(constraintsIndexes[i].second != -2) constraintsIndexes[i].second += objectiveFunction.getNColumns();
    }
    return constraintsIndexes;
}

Matrix LpProblem::solveSimplex() {
    std::pair<uint, uint> pivots = std::make_pair(0, 0);
    std::vector<Matrix> things = initialSimplexTableau();
    Matrix& simplexTableau = things[0];
    Matrix& b = things[1];
    Matrix& cj = things[2];
    Matrix& basisIndices = things[3];
    Matrix& cb = things[4];
    
    Matrix zj = zeros(1, simplexTableau.getNColumns());
    //Matrix cj_minus_zj = zeros(1, simplexTableau.getNColumns());


    unsigned n_surplus_slack_variables = 0;
    for(int i = 0; i < constraints.size(); i++) {
        ConstraintType currentConstraintType = constraints[i].getType();

        if(currentConstraintType == LESS_THAN_OR_EQUAL || currentConstraintType == GREATER_THAN_OR_EQUAL)
            n_surplus_slack_variables++;
    }

    // compute elements of cj - zj row
    for(int i = 0; i < simplexTableau.getNColumns(); i++) {
        Matrix currentColumn = simplexTableau.getColumn(i);
        zj.setElement(0, i, cb.dotProduct(currentColumn));
    }
    Matrix cj_minus_zj = cj - zj;

    uint iterations = 0;
    Matrix ratios = zeros(constraints.size(), 1);
    Matrix pivotColumn;
    while(!isSimplexDone(cj_minus_zj)) {

        pivots.second = cj_minus_zj.maxValueIndex();
        ratios = zeros(constraints.size(), 1);

        pivotColumn = simplexTableau.getColumn(pivots.second);
        ratios = b.pointDivision(pivotColumn);

        // computes the pivot row index
        uint pivotRow = getPivotRow(pivotColumn, b, ratios);
        
        // unbounded problem
        if(pivotRow == -1) {
            status = UNBOUNDED;
            optimalSolution = Matrix({INFINITY}, 1, 1);
            return Matrix({INFINITY}, 1, 1);
        }

        pivots.first = pivotRow;
        uint oldBasis = pivots.first;
        uint newBasis = pivots.second;

        // Remove an artificial variable from the tableau, if variable to leave the basis is artificial
        if(cb.getElement(oldBasis, 0) == M || cb.getElement(oldBasis, 0) == -1 * M) {
            uint artificial_index = basisIndices.getElement(oldBasis, 0);

            simplexTableau.removeColumn(artificial_index);
            cj.removeColumn(artificial_index);
            zj.removeColumn(artificial_index);
            cj_minus_zj.removeColumn(artificial_index);
            
            if(newBasis >= artificial_index) {
                newBasis -= 1;
            }

            for(int i = 0; i < basisIndices.getNRows(); i++) {
                double currentBasisIndex = basisIndices.getElement(i, 0);
                if(currentBasisIndex > artificial_index)
                    basisIndices.setElement(i, 0, currentBasisIndex - 1.0);
            }
        }

        basisIndices.setElement(oldBasis, 0, newBasis);
        cb.setElement(oldBasis, 0, cj.getElement(0, newBasis));

        //Matrix newRow = Matrix(simplexTableau.getRow(oldBasis), 1, simplexTableau.columns()) * (1 / simplexTableau.getElement(oldBasis, newBasis));
        Matrix newRow = simplexTableau.getRow(oldBasis) * (1 / simplexTableau.getElement(oldBasis, newBasis));
        b.setElement(oldBasis, 0, b.getElement(oldBasis, 0) / simplexTableau.getElement(oldBasis, newBasis));

        simplexTableau = simplexTableau.setRow(oldBasis, newRow);

        for(int i = 0; i < simplexTableau.getNRows(); i++) {
            if(i == oldBasis) continue;
            else {
                double factor = simplexTableau.getElement(i, newBasis);
                simplexTableau.rowOperation(oldBasis, i, -1 * factor);
                b.rowOperation(oldBasis, i, -1 * factor);
            }
        }

        for(int i = 0; i < simplexTableau.getNColumns(); i++) {
            Matrix currentColumn = simplexTableau.getColumn(i);
            double innerProduct = cb.dotProduct(currentColumn); 
            zj.setElement(0, i, innerProduct);
        }
        cj_minus_zj = cj - zj;

        iterations++;

    }

    Matrix solution = zeros(1, objectiveFunction.getNColumns());

    // check if problem is infeasible
    double currentBasisIndex;
    for(int k = 0; k < basisIndices.getNRows(); k++) {
        currentBasisIndex = basisIndices.getElement(k, 0);
        if(currentBasisIndex < objectiveFunction.getNColumns()) solution.setElement(0, currentBasisIndex, b.getElement(k, 0));
        else if(currentBasisIndex >= objectiveFunction.getNColumns() + n_surplus_slack_variables && b.getElement(k, 0) > 0) {
            status = INFEASIBLE;
            solution = Matrix({0}, 1, 1);
            break;
        }
    }


    optimalSolution = solution;
    return solution;
}

bool LpProblem::canProblemBeSimplified(SimplifiedConstraintsHelper* helper) {
    
    checkForRepeatedConstraints(helper);

    // Check if any variable's value can be fixed
    canVariablesBeFixed(helper);
    std::vector<uint> repeatedIndexes;
    for(int i = 0; i < helper->constraintsToRemove.size(); i++) {
        for(int j = 0; j < helper->constraintsToRemove.size(); j++) {
            if(helper->constraintsToRemove[i] == helper->constraintsToRemove[j] && j > i) {
                repeatedIndexes.push_back(j);
            }
        }
    }

    std::sort(repeatedIndexes.begin(), repeatedIndexes.end(), std::greater<unsigned>());
    for(int i: repeatedIndexes) {
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

void LpProblem::simplifiedProblemSolution(SimplifiedConstraintsHelper* helper,  Matrix& simplifiedSolution) {

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

    Matrix actualSolution = zeros(1, simplifiedSolution.getNColumns() + helper->fixedVariables.size());

    for(const std::pair<uint, uint>& pairsOfVars : helper->pairsOfVars) {
        actualSolution.setElement(0, pairsOfVars.second, simplifiedSolution.getElement(0, pairsOfVars.first));
    }

    for(const std::pair<uint, double>& fixedVars : helper->fixedVariables) {
        actualSolution.setElement(0, fixedVars.first, fixedVars.second);
    }

    optimalSolution = actualSolution;
}

void LpProblem::canVariablesBeFixed(SimplifiedConstraintsHelper* helper) {
    uint n_vars = objectiveFunction.getNColumns();
    std::vector<std::pair<uint, uint>> basisConstraintsInfo; // .first is the constraint index, .second is the index i in xi <=/=/>= k
    
    // store information of all constraints of the form xi <=/=/>= k in basisConstraintsInfo
    for(int i = 0; i < constraints.size(); i++) {
        int basisVarIndex = isBasisVector(constraints[i].getLhs());
        if(basisVarIndex != -1) basisConstraintsInfo.emplace_back(i, basisVarIndex);
    }

    for(int i = 0; i < basisConstraintsInfo.size(); i++) {
        int currentBasisConstraint_i = basisConstraintsInfo[i].first;

        // Check for constraints of the form xi = k, where i = 1,...,n and k is a real number
        if(constraints[currentBasisConstraint_i].getType() == EQUAL) {
            helper->constraintsToRemove.push_back(currentBasisConstraint_i);
            helper->fixedVariables.push_back(std::make_pair(basisConstraintsInfo[currentBasisConstraint_i].second, constraints[currentBasisConstraint_i].getRhs()));
            //helper->fixedVariables.emplace_back(basisConstraintsInfo[currentBasisConstraint_i].second, constraints[currentBasisConstraint_i].getRhs());
        }

        // checks for any constraint of the types xi <= k and xi >= k   
        for(int j = i + 1; j < basisConstraintsInfo.size(); j++) {
            int currentBasisConstraint_j = basisConstraintsInfo[j].first;
            if(constraints[currentBasisConstraint_i].getLhs() == constraints[currentBasisConstraint_j].getLhs() &&
               constraints[currentBasisConstraint_i].getType() != constraints[currentBasisConstraint_j].getType() &&
               constraints[currentBasisConstraint_i].getRhs() == constraints[currentBasisConstraint_j].getRhs() &&
               (constraints[currentBasisConstraint_i].getType() != EQUAL && constraints[currentBasisConstraint_j].getType() != EQUAL)) {
                    helper->fixedVariables.push_back({basisConstraintsInfo[i].second, constraints[currentBasisConstraint_i].getRhs()});
                    //helper->fixedVariables.emplace_back(basisConstraintsInfo[i].second, constraints[currentBasisConstraint_i].getRhs());
                    helper->constraintsToRemove.push_back(currentBasisConstraint_i);
                    helper->constraintsToRemove.push_back(currentBasisConstraint_j);
            }
        }

        // Check for constraints of the form xi <= 0
        if(constraints[currentBasisConstraint_i].getType() == LESS_THAN_OR_EQUAL && constraints[currentBasisConstraint_i].getRhs() == 0) {
            helper->fixedVariables.push_back({basisConstraintsInfo[i].second, 0});
            //helper->fixedVariables.emplace_back(basisConstraintsInfo[i].second, 0);
            helper->constraintsToRemove.push_back(currentBasisConstraint_i);
        }
    }

    // remove repeated fixed variables
    std::vector<uint> repeatedIndexes;
    for(int i = 0; i < helper->fixedVariables.size(); i++) {
        for(int j = i + 1; j < helper->fixedVariables.size(); j++) {
            if(helper->fixedVariables[i] == helper->fixedVariables[j]) {
                repeatedIndexes.push_back(j);
                //std::cout << j << " ";
            }
        }
    }
    //std::cout << std::endl;

    if(repeatedIndexes.size() > 0) {
        std::sort(repeatedIndexes.begin(), repeatedIndexes.end(), std::greater<unsigned>());
        for(uint i : repeatedIndexes) {
            helper->fixedVariables.erase(helper->fixedVariables.begin() + i);
        }
    }

    std::sort(helper->fixedVariables.begin(), helper->fixedVariables.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
}

void LpProblem::checkForRepeatedConstraints(SimplifiedConstraintsHelper* helper) {
    // Check for repeated constraints
    for(int i = 0; i < constraints.size(); i++) {
        for(int j = i + 1; j < constraints.size(); j++) {
            if(constraints[i] == constraints[j])
                helper->constraintsToRemove.push_back(j);
        }
    }
}

void LpProblem::removeRepeatedFixedVariablesPairs(SimplifiedConstraintsHelper* helper) {
    std::vector<uint> repeatedIndexes;
    for(int i = 0; i < helper->fixedVariables.size(); i++) {
        for(int j = i + 1; j < helper->fixedVariables.size(); j++) {
            if(helper->fixedVariables[i] == helper->fixedVariables[j]) {
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
    for(std::pair<uint, double>& k : helper->fixedVariables) {
        fixedVars.push_back(k.first);
    }

    std::vector<std::pair<uint, uint>> pairsOfVars;
    
    std::vector<uint> freeVars;
    for(uint i = 0; i < objectiveFunction.getNColumns(); i++) {
        if(std::find(fixedVars.begin(), fixedVars.end(), i) == fixedVars.end()) {
            freeVars.push_back(i);
        }
    }

    for(uint i = 0; i < freeVars.size(); i++) {
        //pairsOfVars.push_back(std::make_pair(i, freeVars[i]));
        pairsOfVars.emplace_back(i, freeVars[i]);
    }

    helper->pairsOfVars = pairsOfVars;
}

void LpProblem::removeConstraints(SimplifiedConstraintsHelper* helper) {
    for(uint i : helper->constraintsToRemove) {
        removeConstraint(i);
    }
}

void LpProblem::removeOneFixedVariable(int varIndex, double fixedVarValue) {
    if(varIndex < 0 || varIndex >= objectiveFunction.getNColumns()) {
        std::ostringstream errorMsg;
        errorMsg << "The LP model has " << objectiveFunction.getNColumns() << " variables, but the user tried to remove the variable with index " << varIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    objectiveFunction.removeColumn(varIndex);

    for(uint i = 0; i < constraints.size(); i++) {
        constraints[i].removeFixedVariable(varIndex, fixedVarValue);
    }
}

void LpProblem::removeFixedVariables(SimplifiedConstraintsHelper* helper) {
    for(std::pair<uint, double>& fixedVars : helper->fixedVariables) {
        removeOneFixedVariable(fixedVars.first, fixedVars.second);
    }
}

// PUBLIC METHODS

LpProblem::LpProblem(ProblemType modelType, std::vector<double> newObjectiveFunction, std::vector<Constraint> newConstraints) {
    type = modelType;
    objectiveFunction = Matrix(newObjectiveFunction, 1, newObjectiveFunction.size());
    constraints = newConstraints;
    status = NOT_YET_SOLVED;
    optimalSolution = zeros(1, objectiveFunction.getNColumns());
}

LpProblem::LpProblem(const LpProblem& problem) {
    type = problem.type;
    objectiveFunction = problem.objectiveFunction;
    constraints = problem.constraints;
    optimalSolution = problem.optimalSolution;
    status = problem.status;
}

LpProblem& LpProblem::operator=(const LpProblem& otherProblem) {
    if(this != &otherProblem) {
        type = otherProblem.type;
        objectiveFunction = otherProblem.objectiveFunction;
        constraints = otherProblem.constraints;
        optimalSolution = otherProblem.optimalSolution;
        status = otherProblem.status;
    }
    return *this;
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

void LpProblem::displayProblem() {
    // Objective function
    if(type == MAX) std::cout << "max z: ";
    else if(type == MIN) std::cout << "min z: ";

    for(int i = 0; i < objectiveFunction.getNColumns(); i++) {
        if(objectiveFunction.getElement(0, i) < 0) std::cout << "- ";
        else if(objectiveFunction.getElement(0, i) > 0 && i > 0) std::cout << "+ ";

        if(objectiveFunction.getElement(0, i) != 1 && floor(objectiveFunction.getElement(0, i)) != objectiveFunction.getElement(0, i)) 
            std::cout << std::setprecision(3) << std::fixed << objectiveFunction.getElement(0, i);
        else if(objectiveFunction.getElement(0, i) != 1 && floor(objectiveFunction.getElement(0, i) != 1) == objectiveFunction.getElement(0, i) != 1)
            std::cout << unsigned(objectiveFunction.getElement(0, i));
        std::cout << "x" << i + 1;

        if(i < objectiveFunction.getNColumns() - 1) std::cout << " ";
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
        for(int i = 0; i < optimalSolution.getNColumns(); i++) {
            std::cout << "x" << i + 1;
            if(i < optimalSolution.getNColumns() - 1) std::cout << ", ";
        }
        std::cout << ") = (";
        for(int i = 0; i < optimalSolution.getNColumns(); i++) {
            if(floor(optimalSolution.getElement(0, i)) == optimalSolution.getElement(0, i)) std::cout << unsigned(optimalSolution.getElement(0, i));
            else std::cout << std::setprecision(3) << std::fixed << optimalSolution.getElement(0, i);
            if(i < optimalSolution.getNColumns() - 1) std::cout << ", ";
        }
        std::cout << "), and Z = ";
        if(floor(optimalSolution.dotProduct(objectiveFunction)) == optimalSolution.dotProduct(objectiveFunction)) 
            std::cout << unsigned(optimalSolution.dotProduct(objectiveFunction)) << std::endl;
        else std::cout << std::setprecision(3) << std::fixed << optimalSolution.dotProduct(objectiveFunction) << std::endl;
    }
}

void LpProblem::addConstraint(const Constraint& newConstraint) {
    constraints.push_back(newConstraint);
}

void LpProblem::removeConstraint(int constraintIndex) {
    if(constraintIndex < 0 || constraintIndex >= constraints.size()) {
        std::ostringstream errorMsg;
        errorMsg << "The LP model only has " << constraints.size() << " constraints, but user tried to access constraint with index " << constraintIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    constraints.erase(constraints.begin() + constraintIndex);
}

void LpProblem::solveProblem() {
    SimplifiedConstraintsHelper helper;

    if(canProblemBeSimplified(&helper)) {
        if(helper.fixedVariables.size() == objectiveFunction.getNColumns()) {
	        optimalSolution = zeros(1, objectiveFunction.getNColumns());
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

    for(uint i = 0; i < optimalSolution.getNColumns(); i++) {
        std::pair<bool, double> currentPair = isDoubleAnInteger(optimalSolution.getElement(0, i));
        if(!currentPair.first) {
            status = CONTINUOUS_SOLUTION;
            return false;
        }
        else optimalSolution.setElement(0, i, currentPair.second);
    }
    status = WHOLE_SOLUTION;
    return true;
}

// Non LpProblem class functions

std::pair<bool, double> isDoubleAnInteger(double number, double epsilon) {
    double roundedNumber = std::round(number);
    if(std::abs(number - roundedNumber) < epsilon) {
        return std::make_pair(true, roundedNumber);
    }
    else return std::make_pair(false, number);
}