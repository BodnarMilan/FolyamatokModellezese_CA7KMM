#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define STEPS 6             //  do not modify, these are the required steps of the pizza's journey

#define PIZZAS 6            //  number of pizzas to be made
#define MIN_TIME 1          //  minimum time for the steps
#define MAX_TIME 10         //  maximum time for the steps

#define OVEN_CAPACITY 2     //  pizza baking oven's maximum capacity
                            //  minimum value should be 2 for better efficiency, BUT
                            //  keep at 1 for better demonstration

const char *steps[] =
{
    "Megrendelés",
    "Tészta nyújtás",
    "Szósz hozzáadás",
    "Feltét hozzáadás",
    "Sütés",
    "Szállítás"
};

int main()
{
    int startTime[STEPS][PIZZAS] = {0};
    int endTime[STEPS][PIZZAS] = {0};
    int durations[STEPS][PIZZAS] = {0};

    srand(time(NULL));

    int previousStepEnd[PIZZAS] = {0}; // end of every single pizza's last step

    for (int step = 0; step < STEPS; step++)
    {
        printf("\nLépés %d: %s\n", step + 1, steps[step]);

        // generating random time for every step for every pizza
        for (int i = 0; i < PIZZAS; i++)
        {
            durations[step][i] = MIN_TIME + rand() % (MAX_TIME - MIN_TIME + 1);
        }

        if (step == 0)
        {
            // order - happens sequentially
            //printf("testing step == 0\n");
            int stepStart = 0;
            for (int i = 0; i < PIZZAS; i++)
            {
                if (stepStart < previousStepEnd[i])
                {
                    stepStart = previousStepEnd[i];
                }

                if (i > 0 && stepStart < endTime[step][i - 1])
                {
                    stepStart = endTime[step][i - 1];
                }

                startTime[step][i] = stepStart;
                endTime[step][i] = startTime[step][i] + durations[step][i];
                previousStepEnd[i] = endTime[step][i];

                printf("\tPizza %d - Kezdés: %d, Időtartam: %d, Befejezés: %d\n",
                       i + 1, startTime[step][i], durations[step][i], endTime[step][i]);
            }
        }
        else if (step == 4)
        {
            // cooking - currently limited at most two pizza (int the oven) at the same time for debugging reasons
            // when the limit is hit on currently cooking pizzas, E.g.:
            // pizza 4, 5 and 6 finished step 4 (toppings) at 45, 48 and 50
            // pizza 4 cooks until 54, pizza 5 cooks until 52, but pizza 6 is ready to be cooked at 50
            // in this case pizza 6 waits until one of the pizzas finishes cooking, in this example:
            // pizza 5 is ready at 52 and is replaced by pizza 6
            //printf("testing step == 4\n");
            int baked[PIZZAS] = {0};
            int waitingPrinted[PIZZAS] = {0};
            int waitStartTime[PIZZAS];
            for (int i = 0; i < PIZZAS; i++)
            {
                waitStartTime[i] = -1;
            }

            int pizzasBaked = 0;
            int currentTime = 0;

            while (pizzasBaked < PIZZAS)
            {
                int pizzasInOven = 0;
                for (int i = 0; i < PIZZAS; i++)
                {
                    if (baked[i])
                    {
                        if (startTime[step][i] <= currentTime && endTime[step][i] > currentTime)
                        {
                            pizzasInOven++;
                        }
                    }
                }

                int anyBakedThisCycle = 0;

                for (int i = 0; i < PIZZAS; i++)
                {
                    if (baked[i])
                    {
                        continue;
                    }

                    if (previousStepEnd[i] <= currentTime)
                    {
                        if (pizzasInOven < OVEN_CAPACITY)
                        {
                            // counting the amount of time a pizza has to wait
                            if (waitStartTime[i] != -1 && !waitingPrinted[i])
                            {
                                int waitTime = currentTime - waitStartTime[i];
                                printf("\tPizza %d - Várakozik a sütő tárolóban, Időtartam: %d\n",
                                    i + 1, waitTime);
                                waitingPrinted[i] = 1;
                            }

                            startTime[step][i] = currentTime;
                            endTime[step][i] = startTime[step][i] + durations[step][i];
                            previousStepEnd[i] = endTime[step][i];
                            baked[i] = 1;
                            pizzasBaked++;
                            pizzasInOven++;
                            anyBakedThisCycle = 1;

                            printf("\tPizza %d - Kezdés: %d, Időtartam: %d, Befejezés: %d\n",
                                i + 1, startTime[step][i], durations[step][i], endTime[step][i]);
                        }
                        else
                        {
                            if (waitStartTime[i] == -1)
                            {
                                waitStartTime[i] = currentTime;  // waiting starts here
                            }
                        }
                    }
                }

                if (!anyBakedThisCycle)
                {
                    currentTime++;
                }
            }
        }
        else if (step == 5)
        {
            // delivery - every single pizza gets delivered at the same time regardless of their finish time (actual restaurants do the same)
            //printf("testing step == 5\n");
            int maxPrevEnd = 0;
            for (int i = 0; i < PIZZAS; i++)
            {
                if (previousStepEnd[i] > maxPrevEnd)
                    maxPrevEnd = previousStepEnd[i];
            }

            for (int i = 0; i < PIZZAS; i++)
            {
                startTime[step][i] = maxPrevEnd;
                endTime[step][i] = startTime[step][i] + durations[step][i];
                previousStepEnd[i] = endTime[step][i];

                printf("\tPizza %d - Kezdés: %d, Időtartam: %d, Befejezés: %d\n",
                       i + 1, startTime[step][i], durations[step][i], endTime[step][i]);
            }
        }
        else
        {
            // step by step phase (step 1, 2 and 3 [dough stretching, sauce, toppings])
            // no matter what, you can only work on one pizza during dough stretching (obviously, we only have two hands)
            // spreading sauce and applying toppings can't happen parallelly with multiple pizzas, as it is rare for two pizzas to be exactly the same,
            // not to mention possible mistakes (spillage, dropping ingredients, allergy concerns)
            //printf("testing step by step phase\n");
            int stepStart = 0;
            for (int i = 0; i < PIZZAS; i++)
            {
                if (stepStart < previousStepEnd[i])
                    stepStart = previousStepEnd[i];

                if (i > 0 && stepStart < endTime[step][i - 1])
                    stepStart = endTime[step][i - 1];

                startTime[step][i] = stepStart;
                endTime[step][i] = startTime[step][i] + durations[step][i];
                previousStepEnd[i] = endTime[step][i];

                printf("\tPizza %d - Kezdés: %d, Időtartam: %d, Befejezés: %d\n",
                       i + 1, startTime[step][i], durations[step][i], endTime[step][i]);
            }
        }
    }

    return 0;
}
