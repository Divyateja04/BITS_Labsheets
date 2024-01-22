#include <bits/stdc++.h>
#define endl "\n"
using namespace std;

// GLOBAL VARIABLES
int stateCounter = 0;

class State
{
public:
    int number;
    vector<State *> a;
    vector<State *> b;
    vector<State *> e;

    State()
    {
        number = stateCounter++;
        a = {};
        b = {};
        e = {};
    }
};

class StateSet
{
public:
    State *start;
    State *end;

    StateSet()
    {
        this->start = new State();
        this->end = new State();
    }

    StateSet(char x)
    {
        this->start = new State();
        this->end = new State();
        if (x == 'a')
            this->start->a.push_back(this->end);
        if (x == 'b')
            this->start->b.push_back(this->end);
    }

    void OR(StateSet *first, StateSet *second)
    {
        // Make the Start -> first -> end loop
        this->start->e.push_back(first->start);
        first->end->e.push_back(this->end);

        // Make the Start -> second -> end loop
        this->start->e.push_back(second->start);
        second->end->e.push_back(this->end);
    }

    void CONCAT(StateSet *first, StateSet *second)
    {
        // Make the Start -> first
        this->start->e.push_back(first->start);
        // first -> second
        first->end->e.push_back(second->start);
        // second -> end
        second->end->e.push_back(this->end);
    }

    void KLEENESTAR(StateSet *top)
    {
        // Start -> top.start
        this->start->e.push_back(top->start);
        // top.end -> start
        top->end->e.push_back(this->start);
        // start -> end
        this->start->e.push_back(this->end);
    }

    map<int, vector<vector<int>>> performBFS()
    {
        // Map to keep track of all transitions
        map<int, vector<vector<int>>> stateTransitions;
        // Used to perform BFS and keep track of repetitive states
        queue<State *> q;
        map<int, int> mp;

        q.push(this->start);
        mp[this->start->number] = 1;

        while (!q.empty())
        {
            State *curr = q.front();
            q.pop();

            mp[curr->number] = 1;

            // cout << "::> State Number: " << curr->number << endl;
            // Initialize, corresponding to a | b | e
            stateTransitions[curr->number] = {{}, {}, {}};

            for (auto x : curr->a)
            {
                stateTransitions[curr->number][0].push_back(x->number);
                if (!mp[x->number])
                    q.push(x);
            }
            for (auto x : curr->b)
            {
                stateTransitions[curr->number][1].push_back(x->number);
                if (!mp[x->number])
                    q.push(x);
            }
            for (auto x : curr->e)
            {
                stateTransitions[curr->number][2].push_back(x->number);
                if (!mp[x->number])
                    q.push(x);
            }
        }

        return stateTransitions;
    }
};

// Function to return precedence of operators
int prec(char c)
{
    if (c == '*')
        return 3;
    else if (c == '.')
        return 2;
    else if (c == '|')
        return 1;
    else
        return -1;
}

// Function to convert infix to Post fix
string infixToPostfix(string input)
{
    string postfix;

    stack<char> s;
    for (auto x : input)
    {
        // If character is operator print
        if (('a' <= x && x <= 'z') || (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9'))
        {
            postfix += x;
        }
        else if (x == '(')
            s.push(x);
        else if (x == ')')
        {
            while (s.top() != '(')
            {
                postfix += s.top();
                s.pop();
            }
            s.pop();
        }
        else
        {
            while (!s.empty() && prec(x) < prec(s.top()) || !s.empty() && prec(x) == prec(s.top()))
            {
                postfix += s.top();
                s.pop();
            }
            s.push(x);
        }
    }

    // Pop all the remaining elements from the stack
    while (!s.empty())
    {
        postfix += s.top();
        s.pop();
    }

    // Return the postfix expression
    return postfix;
}

StateSet *createNFA(string postfix)
{
    stack<StateSet *> s;
    for (auto x : postfix)
    {
        if (x == 'a' || x == 'b')
            s.push(new StateSet(x));
        else if (x == '|')
        {
            cout << "::> Found an OR" << endl;
            StateSet *first = s.top();
            s.pop();
            StateSet *second = s.top();
            s.pop();
            StateSet *final = new StateSet();
            final->OR(first, second);
            s.push(final);
        }
        else if (x == '.')
        {
            cout << "::> Found a CONCAT" << endl;
            StateSet *first = s.top();
            s.pop();
            StateSet *second = s.top();
            s.pop();
            StateSet *final = new StateSet();
            final->CONCAT(first, second);
            s.push(final);
        }
        else if (x == '*')
        {
            cout << "::> Found a KLEENE STAR" << endl;
            StateSet *top = s.top();
            s.pop();
            StateSet *final = new StateSet();
            final->KLEENESTAR(top);
            s.push(final);
        }
    }

    if (s.empty() || s.size() > 1)
    {
        cout << "ERROR OCCURRED: Found " << s.size() << " elements." << endl;
        return nullptr;
    }
    else
    {
        return s.top();
    }
}

bool runTheModel(string regexp, string testcase)
{
    cout << "Reg Exp. " << regexp << endl;
    cout << "Testcase: " << testcase << endl;

    // Step 1: Convert infix to Postfix
    string postfix = infixToPostfix(regexp);
    cout << "Postfix: " << postfix << endl;

    // Step 2: Generate the State Set Structure
    StateSet *output = createNFA(postfix);

    // Step 3: Perform BFS on this structure
    map<int, vector<vector<int>>> stateTransitions = output->performBFS();

    // Step 4: Print the NFA with e transitions
    cout << "::> Found the following NFA with e Transitions: " << endl;
    for (int i = 0; i < stateTransitions.size(); i++)
    {
        cout << "::> State: " << i << " ";
        for (auto x : stateTransitions[i])
        {
            cout << "{ ";
            for (auto y : x)
            {
                cout << y << ' ';
            }
            cout << "} ";
        }
        cout << endl;
    }

    // Step 5: Convert NFA to DFA

    return true;
}

int main()
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    int counter;
    cin >> counter;

    for (int m = 0; m < counter; m++)
    {
        cout << "----------- TEST CASE " << m + 1 << " -----------" << endl;

        string re;
        string input;
        cin >> re >> input;

        if (runTheModel(re, input))
        {
            cout << "Output: YES" << endl;
        }
        else
        {
            cout << "Output: NO" << endl;
        }

        cout << "::> Resetting State numbers" << endl;
        stateCounter = 0;
        cout << "--------- END TEST CASE "
             << " ---------" << endl
             << endl;
    }
}