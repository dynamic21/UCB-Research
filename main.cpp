#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>
#define MaxSlots 200
#define MinSlots 200
#define MaxAverageScore 1000
#define MinAverageScore 0
#define MaxScoreDeviation 100
#define MinScoreDeviation 0
#define NumberOfEquations 100
#define learningRate 0.2

using std::cout;
using std::endl;
using std::log;
using std::sort;
using std::vector;

double randDouble(double max = 1, double min = 0)
{
    return (double)rand() / RAND_MAX * (max - min) + min;
}

int randInt(int max = 1, int min = 0)
{
    return rand() % (max - min + 1) + min;
}

class slot
{
public:
    double averageScore;
    double scoreDeviation;

    slot()
    {
        averageScore = randDouble(MaxAverageScore, MinAverageScore);
        scoreDeviation = randDouble(MaxScoreDeviation, MinScoreDeviation);
    }

    void printRange()
    {
        cout << "(" << averageScore - scoreDeviation << ") - (" << averageScore + scoreDeviation << ")";
    }

    double getScore()
    {
        return averageScore + randDouble(1, -1) * scoreDeviation;
    }
};

class equation
{
public:
    double var1;
    double var2;
    double var3;
    double totalScore;
    int tries;

    equation()
    { 
        var1 = 1;
        var2 = 1.5;
        var3 = 2;
        totalScore = 0;
        tries = 0;
    }

    equation(equation *givenEquation)
    {
        var1 = givenEquation->var1;
        var2 = givenEquation->var2;
        var3 = givenEquation->var3;
        totalScore = givenEquation->totalScore;
        tries = givenEquation->tries;
    }

    void info()
    {
        cout << "Vars:" << var1 << " & " << var2 << " & " << var3 << endl;
        cout << "score:" << totalScore / tries << endl;
    }

    void mutate()
    {
        var1 += randDouble(learningRate, -learningRate);
        var2 += randDouble(learningRate, -learningRate);
        var3 += randDouble(learningRate, -learningRate);
    }

    double getBias(double totalScore, int totalTries, int maxTries)
    {
        return (totalScore / totalTries + var1 * sqrt((var2 * log(var3 * maxTries) / totalTries)));
    }
};

class code
{
public:
    vector<slot> slots;
    vector<equation *> equations;

    code()
    {
        int i = NumberOfEquations;
        while (i--)
        {
            equations.push_back(new equation());
        }
    }

    void info()
    {
        for (int i = 0; i < slots.size(); i++)
        {
            cout << "Range of slot " << i << " is ";
            slots[i].printRange();
            cout << endl;
        }
    }

    void createNewSlots()
    {
        slots.clear();
        int i = randInt(MaxSlots, MinSlots); // num of slots
        while (i--)
        {
            slot newSlot;
            slots.push_back(newSlot);
        }
    }

    void scoreEquation(equation *givenEquation)
    {
        int maxTries = slots.size() * 10;
        vector<int> totalScore(slots.size(), 0);
        vector<int> tries(slots.size(), 0);
        for (int i = 0; i < maxTries; i++)
        {
            // if (i % 100 == 0)
            // {
            //     cout << (100 * i / maxTries) << endl;
            // }
            double maxBias = 0;
            double currentIndex = 0;
            for (int j = 0; j < slots.size(); j++)
            {
                if (tries[j] == 0)
                {
                    currentIndex = j;
                    break;
                }
                double bias = givenEquation->getBias(totalScore[j], tries[j], maxTries);
                if (bias > maxBias)
                {
                    maxBias = bias;
                    currentIndex = j;
                }
            }
            tries[currentIndex]++;
            totalScore[currentIndex] += slots[currentIndex].getScore();
        }
        double score = 0;
        for (int i = 0; i < slots.size(); i++)
        {
            // cout << tries[i] << " & " << slots[i].averageScore << endl;
            score -= tries[i] * (slots[0].averageScore - slots[i].averageScore);
        }
        givenEquation->totalScore += score;
        givenEquation->tries++;
    }

    void sortSlots()
    {
        sort(slots.begin(), slots.end(), [](slot slot1, slot slot2) { // sort from greatest to least
            return (slot1.averageScore > slot2.averageScore);
        });
    }

    void sortEquations()
    {
        sort(equations.begin(), equations.end(), [](equation *equation1, equation *equation2) { // sort from greatest to least
            return (equation1->totalScore / equation1->tries > equation2->totalScore / equation2->totalScore);
        });
    }

    void evolve()
    {
        equation *newEquation = equations[0];
        newEquation->info();
        newEquation->totalScore = 0;
        newEquation->tries = 0;
        for (int i = 1; i < NumberOfEquations; i++)
        {
            delete equations[i];
        }
        equations.clear();
        for (int i = 0; i < NumberOfEquations; i++)
        {
            equations.push_back(new equation(newEquation));
            equations[i]->mutate();
        }
        delete newEquation;
    }
};

int main()
{
    srand(time(NULL));

    code NewCode;

    while (true)
    {
        NewCode.createNewSlots();
        NewCode.sortSlots();
        for (int i = 0; i < NumberOfEquations; i++)
        {
            NewCode.scoreEquation(NewCode.equations[i]);
        }
        NewCode.evolve();
    }
}