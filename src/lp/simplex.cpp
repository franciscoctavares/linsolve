#include "lp/simplex.hpp"
#include "lp/lp_simplifier.hpp"
#include "lp/lp_utils.hpp"

#include <sstream>
#include <iostream>
#include <cmath>
#include <format>
#include <cstddef>

namespace LP {
    
// PRIVATE METHODS

bool SimplexSolver::isSimplexDone(Matrix& cj_minus_zj) {
    for(std::size_t i = 0; i < cj_minus_zj.getNColumns(); i++) {
        if(cj_minus_zj.getElement(0, i) > 0) return false;
    }
    return true;
}

std::size_t SimplexSolver::getPivotRow(Matrix& simplexAux, Matrix& bAux) {
    double minValue = M;
    unsigned minIndex = 0;
    unsigned invalid = 0;
    double ratioElement;

    for(std::size_t i = 0; i < simplexAux.getNRows(); i++) {
        if(simplexAux.getElement(i, 0) <= 0) invalid++;
    }
    if(invalid == simplexAux.getNRows()) return -1;

    for(std::size_t i = 0; i < simplexAux.getNRows(); i++) {
        ratioElement = bAux.getElement(i, 0) / simplexAux.getElement(i, 0);
        if(ratioElement < minValue && ratioElement > 0 && ratioElement != M) {
            minValue = ratioElement;
            minIndex = i;
        }
    }

    return minIndex;
}

Matrix SimplexSolver::getBasisIndexes(Matrix& extraCj) {
    std::vector<std::pair<int, int>> restrictionsIndices(problem.getConstraints().size(), {-1, -1});
    std::vector<ConstraintType> constraintsTypes;

    for(const Constraint& currentConstraint : problem.getConstraints()) {
        constraintsTypes.push_back(currentConstraint.getType());
    }


    unsigned n_slack_surplus_variables = 0;
    unsigned totalExtraVariables = extraCj.getNColumns();
    unsigned currentCoefficient = 0;
    for(std::size_t i = 0; i < problem.getConstraints().size(); i++) {
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
    for(std::size_t i = 0; i < constraintsTypes.size(); i++) {
        /*
        if(restrictionsIndices[i].second == -2) continue;
        else if(restrictionsIndices[i].second == -1) artificialRestrictions.push_back(i);
        */
        if(restrictionsIndices[i].second == -1) artificialRestrictions.push_back(i);
    }

    for(std::size_t i = n_slack_surplus_variables; i < totalExtraVariables; i++) {
        if(artificialRestrictions.size() > 0) {
            restrictionsIndices[artificialRestrictions[0]].second = i;
            artificialRestrictions.erase(artificialRestrictions.begin());
        }
    }

    for(std::pair<int, int>& currentPair : restrictionsIndices) {
        if(currentPair.first != -2) currentPair.first += problem.getObjectiveFunction().getNColumns();
        if(currentPair.second != -2) currentPair.second += problem.getObjectiveFunction().getNColumns();
    }

    std::vector<double> basisIndices;
    for(std::size_t i = 0; i < constraintsTypes.size(); i++) {
        if(constraintsTypes[i] == LESS_THAN_OR_EQUAL) basisIndices.push_back(restrictionsIndices[i].first);
        else basisIndices.push_back(restrictionsIndices[i].second);
        /*
        else if(constraintsTypes[i] == GREATER_THAN_OR_EQUAL) basisIndices.push_back(restrictionsIndices[i].second);
        else if(constraintsTypes[i] == EQUAL) basisIndices.push_back(restrictionsIndices[i].second);
        */
    }
    
    return Matrix(basisIndices, constraintsTypes.size(), 1);
}

Matrix SimplexSolver::getConstraintsLHS() {
    Matrix aux(problem.getConstraints()[0].getLhs(), 1, problem.getConstraints()[0].getLhs().size());
    for(std::size_t i = 1; i < problem.getConstraints().size(); i++) {
        Matrix currentRow = Matrix(problem.getConstraints()[i].getLhs(), 1, aux.getNColumns());
        aux.stackVertical(currentRow);
    }
    return aux;
}

std::vector<ConstraintType> SimplexSolver::getConstraintsTypes() {
    std::vector<ConstraintType> aux;
    for(std::size_t i = 0; i < problem.getConstraints().size(); i++)
        aux.push_back(problem.getConstraints()[i].getType());
    return aux;
}

Matrix SimplexSolver::getConstraintsRHS() {
    std::vector<double> aux;
    for(std::size_t i = 0; i < problem.getConstraints().size(); i++)
        aux.push_back(problem.getConstraints()[i].getRhs());
    return Matrix(aux, aux.size(), 1);
}

bool SimplexSolver::isConstraintSatisfied(Matrix potentialSolution, int constraintIndex) {
    if(constraintIndex < 0 || static_cast<std::size_t>(constraintIndex) >= problem.getConstraints().size()) {
        std::ostringstream errorMsg;
        errorMsg << "The LP model only has " << problem.getConstraints().size() << " constraints, but user tried to access constraint with index " << constraintIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    Matrix constraintLhs(problem.getConstraints()[constraintIndex].getLhs(), 1, problem.getConstraints()[constraintIndex].getLhs().size());
    ConstraintType restType = problem.getConstraints()[constraintIndex].getType();
    double rhs = problem.getConstraints()[constraintIndex].getRhs();
    double value = potentialSolution.dotProduct(constraintLhs);
    
    if(restType == LESS_THAN_OR_EQUAL) return (value <= rhs) ? true : false;    // <=
    else if(restType == EQUAL) return (value == rhs) ? true : false;            // =
    else return (value >= rhs) ? true : false;                                  // >=
}

bool SimplexSolver::isSolutionAdmissible(Matrix potentialSolution) {
    for(std::size_t i = 0; i < problem.getConstraints().size(); i++) {
        if(!isConstraintSatisfied(potentialSolution, i))
            return false;
    }

    // non negativity
    for(std::size_t i = 0; i < potentialSolution.getNColumns(); i++) {
        if(potentialSolution.getElement(0, i) < 0)
            return false;
    }

    return true;
}

Matrix SimplexSolver::extraVariablesMatrix() {
    unsigned nVariables = 0;
    unsigned slack_surplus_variables = 0;
    unsigned artificial_variables = 0;

    std::vector<std::pair<std::size_t, double>> pairs;

    for(std::size_t i = 0; i < problem.getConstraints().size(); i++) {
        if(problem.getConstraints()[i].getType() == LESS_THAN_OR_EQUAL) {
            nVariables += 1;
            slack_surplus_variables += 1;
        }
        else if(problem.getConstraints()[i].getType() == EQUAL) {
            nVariables += 1;
            artificial_variables += 1;
        }
        else if(problem.getConstraints()[i].getType() == GREATER_THAN_OR_EQUAL) {
            nVariables += 2;
            slack_surplus_variables += 1;
            artificial_variables += 1;
        }
    }

    for(std::size_t i = 0; i < problem.getConstraints().size(); i++) {
        if(problem.getConstraints()[i].getType() == LESS_THAN_OR_EQUAL) {
            pairs.emplace_back(i, 1.0);
        }
        else if(problem.getConstraints()[i].getType() == EQUAL) {
            continue;
        }
        else if(problem.getConstraints()[i].getType() == GREATER_THAN_OR_EQUAL) {
            pairs.emplace_back(i, -1.0);
        }
    }

    for(std::size_t i = 0; i < problem.getConstraints().size(); i++) {
        if(problem.getConstraints()[i].getType() == EQUAL || problem.getConstraints()[i].getType() == GREATER_THAN_OR_EQUAL) {
            pairs.emplace_back(static_cast<double>(i), 1.0);
        }
        else continue;
        /*
        if(problem.getConstraints()[i].getType() != LESS_THAN_OR_EQUAL)
            pairs.emplace_back(i, 1.0);
        */
    }

    Matrix aux = zeros(problem.getConstraints().size(), nVariables);

    for(std::size_t i = 0; i < pairs.size(); i++) {
        Matrix currentBasisVector = basisVector(problem.getConstraints().size(), pairs[i].first) * pairs[i].second;
        aux.setColumn(i, currentBasisVector);
    }

    return aux;
}

std::vector<Matrix> SimplexSolver::initialSimplexTableau() {
    Matrix simplexTableau = getConstraintsLHS();

    Matrix extraVars = extraVariablesMatrix();    
    simplexTableau.stackHorizontal(extraVars);

    Matrix b = getConstraintsRHS();

    Matrix cj = problem.getObjectiveFunction();
    if(problem.getType() == MIN) cj *= -1;

    std::vector<double> aux;

    for(const Constraint& currentConstraint : problem.getConstraints()) {
        if(currentConstraint.getType() != EQUAL) aux.push_back(0.0);
    }

    std::vector<double> basisCoeffs;
    for(const Constraint& currentConstraint : problem.getConstraints()) {        
        if(currentConstraint.getType() == LESS_THAN_OR_EQUAL) {
            basisCoeffs.push_back(0.0);
        }
        else {
            aux.push_back(-1 * M);
            basisCoeffs.push_back(-1 * M);   
        }        
    }

    Matrix extraCj(aux, 1, aux.size());
    Matrix basisIndicesAux = getBasisIndexes(extraCj);
    cj.stackHorizontal(extraCj);
    Matrix cb(basisCoeffs, basisCoeffs.size(), 1);

    return {simplexTableau, b, cj, basisIndicesAux, cb};
}

std::vector<std::pair<int, int>> SimplexSolver::getConstraintsIndexes(Matrix extraCj) {
    std::vector<std::pair<int, int>> constraintsIndexes(problem.getConstraints().size(), {-1, -1});
    //for(std::size_t i = 0; i < problem.getConstraints().size(); i++) constraintsIndexes.push_back(std::make_pair(-1, -1));


    unsigned n_slack_surplus_variables = 0;
    unsigned totalExtraVariables = extraCj.getNColumns();
    unsigned currentCoefficient = 0;
    for(std::size_t i = 0; i < problem.getConstraints().size(); i++) {
        if(problem.getConstraints()[i].getType() == LESS_THAN_OR_EQUAL) {
            constraintsIndexes[i].first = currentCoefficient;
            currentCoefficient++;
            constraintsIndexes[i].second = -2; // nas restrições <= não há variáveis artificiais
            n_slack_surplus_variables++;
        }
        else if(problem.getConstraints()[i].getType() == GREATER_THAN_OR_EQUAL) {
            constraintsIndexes[i].first = currentCoefficient;
            currentCoefficient++;
            n_slack_surplus_variables++;
        }
        else if(problem.getConstraints()[i].getType() == EQUAL) {
            constraintsIndexes[i].first = -2; // nas restrições = não há coeficiente do zero
        }
    }

    std::vector<unsigned> artificialRestrictions;
    for(std::size_t i = 0; i < problem.getConstraints().size(); i++) {
        if(constraintsIndexes[i].second == -2) continue;
        else if(constraintsIndexes[i].second == -1) artificialRestrictions.push_back(i);
    }

    for(std::size_t i = n_slack_surplus_variables; i < totalExtraVariables; i++) {
        if(artificialRestrictions.size() > 0) {
            constraintsIndexes[artificialRestrictions[0]].second = i;
            artificialRestrictions.erase(artificialRestrictions.begin());
        }
    }

    for(std::size_t i = 0; i < constraintsIndexes.size(); i++) {
        if(constraintsIndexes[i].first != -2) constraintsIndexes[i].first += problem.getObjectiveFunction().getNColumns();
        if(constraintsIndexes[i].second != -2) constraintsIndexes[i].second += problem.getObjectiveFunction().getNColumns();
    }
    return constraintsIndexes;
}

void SimplexSolver::solveSimplex() {
    std::pair<uint, uint> pivots = std::make_pair(0, 0);

    std::vector<Matrix> things = initialSimplexTableau();
    Matrix& simplexTableau = things[0];
    Matrix& b = things[1];
    Matrix& cj = things[2];
    Matrix& basisIndices = things[3];
    Matrix& cb = things[4];

    Matrix zj = zeros(1, simplexTableau.getNColumns());

    unsigned n_surplus_slack_variables = 0;
    for(const Constraint& currentConstraint : problem.getConstraints()) {
        if(currentConstraint.getType() != EQUAL)
            n_surplus_slack_variables++;
    }

    // compute elements of cj - zj row
    for(std::size_t i = 0; i < simplexTableau.getNColumns(); i++) {
        Matrix currentColumn = simplexTableau.getColumn(i);
        zj.setElement(0, i, cb.dotProduct(currentColumn));
    }
    Matrix cj_minus_zj = cj - zj;

    uint iterations = 0;
    Matrix ratios = zeros(problem.getConstraints().size(), 1);
    Matrix pivotColumn;
    while(!isSimplexDone(cj_minus_zj)) {

        if(iterations > 1000) {
            std::cout << "I seem to be getting stuck here..." << std::endl;
            cj_minus_zj.displayMatrix();
            std::cout << std::endl;
            ratios.displayMatrix();
            std::cout << std::endl;
        }
        
        // compute pivot column index
        pivots.second = computePivotColumn(cj_minus_zj);
        pivotColumn = simplexTableau.getColumn(pivots.second);

        // compute pivot row index
        uint pivotRow = getPivotRow(pivotColumn, b);
        
        // unbounded problem
        if(static_cast<int>(pivotRow) == -1) {
            problem.setSolutionType(UNBOUNDED);
            problem.setOptimalSolution(Matrix({INFINITY}, 1, 1));
            return;
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

            for(std::size_t i = 0; i < basisIndices.getNRows(); i++) {
                double currentBasisIndex = basisIndices.getElement(i, 0);
                if(currentBasisIndex > artificial_index)
                    basisIndices.setElement(i, 0, currentBasisIndex - 1.0);
            }
        }

        basisIndices.setElement(oldBasis, 0, newBasis);
        cb.setElement(oldBasis, 0, cj.getElement(0, newBasis));

        //Matrix newRow = Matrix(simplexTableau.getRow(oldBasis), 1, simplexTableau.columns()) * (1 / simplexTableau.getElement(oldBasis, newBasis));
        double pivotElement = simplexTableau.getElement(oldBasis, newBasis);
        //Matrix newRow = simplexTableau.getRow(oldBasis) * (1 / pivotElement);
        b.setElement(oldBasis, 0, b.getElement(oldBasis, 0) / pivotElement);

        simplexTableau.setRow(oldBasis, simplexTableau, 1 / pivotElement);

        // perform row operations
        for(std::size_t i = 0; i < simplexTableau.getNRows(); i++) {
            if(i == oldBasis) continue;
            else {
                double factor = simplexTableau.getElement(i, newBasis);
                simplexTableau.rowOperation(oldBasis, i, -1 * factor);
                b.rowOperation(oldBasis, i, -1 * factor);
            }
        }

        for(std::size_t i = 0; i < simplexTableau.getNColumns(); i++) {
            Matrix currentColumn = simplexTableau.getColumn(i);
            double innerProduct = cb.dotProduct(currentColumn); 
            zj.setElement(0, i, innerProduct);
        }

        cj_minus_zj = cj - zj;

        iterations++;
    }

    Matrix solution = zeros(1, problem.getObjectiveFunction().getNColumns());

    // check if problem is infeasible
    double currentBasisIndex;
    for(std::size_t k = 0; k < basisIndices.getNRows(); k++) {
        currentBasisIndex = basisIndices.getElement(k, 0);
        if(currentBasisIndex < problem.getObjectiveFunction().getNColumns())
            solution.setElement(0, currentBasisIndex, b.getElement(k, 0));
        else if(currentBasisIndex >= problem.getObjectiveFunction().getNColumns() + n_surplus_slack_variables && b.getElement(k, 0) > 0) {
            problem.setSolutionType(INFEASIBLE);
            //solution = Matrix({0}, 1, 1);

            problem.setOptimalSolution(Matrix({0}, 1, 1));
            isOptimalSolutionWhole();
            return;
        }
    }

    problem.setOptimalSolution(solution);
    isOptimalSolutionWhole();
}

void SimplexSolver::isOptimalSolutionWhole() {
    Matrix infeasibleSol({0}, 1, 1);
    Matrix unboundedSol({INFINITY}, 1, 1);

    if(problem.getOptimalSolution() == infeasibleSol) {
        problem.setSolutionType(INFEASIBLE);
    }
    else if(problem.getOptimalSolution() == unboundedSol) {
        problem.setSolutionType(UNBOUNDED);
    }

    for(uint i = 0; i < problem.getOptimalSolution().getNColumns(); i++) {
        //std::pair<bool, double> currentPair = isDoubleAnInteger(problem.getOptimalSolution().getElement(0, i));
        if(!isNumberAnInteger(problem.getOptimalSolution().getElement(0, i))) {
            problem.setSolutionType(CONTINUOUS_SOLUTION);
            return;
        }
    }
    problem.setSolutionType(WHOLE_SOLUTION);
}

void SimplexSolver::computeOriginalProblemSolution(std::pair<const Matrix&, SolutionType>& simplifiedResult, SimplifierHelper& helper) {
    if(simplifiedResult.second == INFEASIBLE) {
        problem.setSolutionType(INFEASIBLE);
        problem.setOptimalSolution(simplifiedResult.first);
    }
    else if(simplifiedResult.second == UNBOUNDED) {
        problem.setSolutionType(UNBOUNDED);
        problem.setOptimalSolution(simplifiedResult.first);
    }
    else if(simplifiedResult.second == CONTINUOUS_SOLUTION) {
        problem.setSolutionType(CONTINUOUS_SOLUTION);

        if(helper.fixedVariables.size() > 0) {
            for(const std::pair<uint, uint>& pairsOfVars : helper.pairsOfVars) {
                problem.getOptimalSolution().setElement(0, pairsOfVars.second, simplifiedResult.first.getElement(0, pairsOfVars.first));
            }

            for(const std::pair<uint, double>& fixedVars : helper.fixedVariables) {
                problem.getOptimalSolution().setElement(0, fixedVars.first, fixedVars.second);
            }
        }
    }

    if(simplifiedResult.second == WHOLE_SOLUTION) {
        problem.setSolutionType(WHOLE_SOLUTION);

        if(helper.fixedVariables.size() > 0) {
            for(const std::pair<uint, uint>& pairsOfVars : helper.pairsOfVars) {
                problem.getOptimalSolution().setElement(0, pairsOfVars.second, simplifiedResult.first.getElement(0, pairsOfVars.first));
            }

            for(const std::pair<uint, double>& fixedVars : helper.fixedVariables) {
                problem.getOptimalSolution().setElement(0, fixedVars.first, fixedVars.second);
            }
        }
    }
}

std::size_t SimplexSolver::computePivotColumn(Matrix& cj_minus_zj) {
    std::vector<std::pair<std::size_t, double>> validIndexes;
    std::vector<std::size_t> zeroIndexes;

    for(size_t i = 0; i < cj_minus_zj.getNColumns(); i++) {
        if(cj_minus_zj.getElement(0, i) == 0) {
            zeroIndexes.push_back(i);
            validIndexes.push_back({i, cj_minus_zj.getElement(0, i)});
        }
        else if(cj_minus_zj.getElement(0, i) > 0) {
            validIndexes.push_back({i, cj_minus_zj.getElement(0, i)});
        }
    }

    //std::cout << std::format("valid({}), zeros({})", validIndexes.size(), zeroIndexes.size()) << std::endl;
    /*
    for(size_t i = 0; i < validIndexes.size(); i++) {
        std::cout << validIndexes[i].second << " ";
    }
    std::cout << std::endl;
    for(size_t i = 0; i < zeroIndexes.size(); i++) {
        std::cout << cj_minus_zj.getElement(0, zeroIndexes[i]) << " ";
    }
    std::cout << std::endl;
    */
    /*
    if(zeroIndexes.size() > 0 && validIndexes.size() == zeroIndexes.size()) {
        //std::cout << "Bland's rule..." << std::endl;
        return validIndexes[0].first;
    }
    else {
        //std::cout << "Normal pivoting..." << std::endl;
        return cj_minus_zj.maxValueIndex();
    }
    */
    return cj_minus_zj.maxValueIndex();
}

// PUBLIC METHODS

std::pair<Matrix, SolutionType> SimplexSolver::runSolver() {
    SimplifierHelper helper = LPSimplifier::computeSimplifierHelper(problem);

    if(helper.isHelperEmpty()) {
        solveSimplex();
        return std::make_pair(problem.getOptimalSolution(), problem.getSolutionType());
    }
    else {
        LpProblem simplifiedProblem(problem);

        LPSimplifier::simplifyProblem(simplifiedProblem, helper);

        SimplexSolver simplifiedSolver(simplifiedProblem);
        simplifiedSolver.solveSimplex();

        std::pair<const Matrix&, SolutionType> simplifiedResult = {simplifiedProblem.getOptimalSolution(), simplifiedProblem.getSolutionType()};

        computeOriginalProblemSolution(simplifiedResult, helper);

        return std::make_pair(problem.getOptimalSolution(), problem.getSolutionType());
    }
}

}