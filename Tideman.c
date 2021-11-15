#include <cs50.h>
#include <stdio.h>
#include <string.h>
// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int *getAllLosersForWinner(int winner, int losers[]);
bool hasCycle(int winner);
int visited[MAX];
int visiting[MAX];

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }




    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        int candidateIdx = ranks[i];
        for (int j = i + 1; j < candidate_count; j++)
        {
            int nextCandidateIdx = ranks[j];
            preferences[candidateIdx][nextCandidateIdx]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    pair_count = 0;
    for (int i = 0; i < candidate_count ; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int hasSwaps = 0;
    do
    {
        int swaps = 0;
        for (int i = 0; i < pair_count - 1; i++)
        {
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[i + 1].winner][pairs[i + 1].loser])
            {
                swaps++;
                pair tmpPair = pairs[i];
                pairs[i] = pairs[i + 1];
                pairs[i + 1] = tmpPair;
            }
        }

        hasSwaps = swaps > 0;

    }
    while (hasSwaps);
    return;
}

int *getAllLosersForWinner(int winner, int losers[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        losers[i] = 0;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[winner][i])
        {
            losers[i] = 1;
        }
    }
    return losers;
}

bool hasCycle(int winner)
{
    visiting[winner] = 1;
    int losers[candidate_count];
    int tmp = winner;
    getAllLosersForWinner(winner, losers);
    for (int i = 0; i < candidate_count; i++)
    {
        if (losers[i] == 1)
        {
            //this is a loser of the winner
            if (visiting[i] == 1)
            {
                // this loser is already in the stack
                return true;
            }
            if (visited[i] != 1 && hasCycle(i))
            {
                // recursive check descendents
                return true;
            }
        }
    }
    // done visiting this winner's descendents - backtrack
    visiting[tmp] = 0;
    visited[tmp] = 1;
    return false;
}



// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < MAX; i++)
    {
        visiting[i] = 0;
        visited[i] = 0;
    }
    //lock in strongest win margin for a pair
    for (int i = 0; i < pair_count; i++)
    {
        int currentWinner = pairs[i].winner;
        int currentLoser = pairs[i].loser;
        // set edge
        locked[currentWinner][currentLoser] = true;
        // test for cycle
        if (hasCycle(currentWinner))
        {
            // unset edge since it is creating a cycle
            //printf("found cycle from %i to %i\n", currentWinner, currentLoser);
            locked[currentWinner][currentLoser] = false;
        }
        for (int j = 0; j < MAX; j++)
        {
            visiting[j] = 0;
            visited[j] = 0;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        int hasEdge = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                hasEdge = 1;
            }
        }
        if (!hasEdge)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}