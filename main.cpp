#include<bits/stdc++.h>
using namespace std;

unordered_map<string,vector<string>> graph;
unordered_map<string,vector<string>> incoming_links;
unordered_map<string,int> impressions;
unordered_map<string,int> CTR;
unordered_map<string,double> pagerank;
unordered_map<string,vector<string>> search_table;

void getGraph()
{
    fstream fin;
    string colname, line;
    fin.open("webGraph.csv");
    while(getline(fin, line))
    {
        stringstream ss(line);
        bool flag = true;
        string start;
        vector<string> adj;
        while(getline(ss, colname, ','))
        {
            if(flag)
            {
                start = colname;
                flag = false;
            }
            else
            {
                adj.push_back(colname);
                incoming_links[colname].push_back(start);
            }
        }
        graph[start]=adj;
    }
    fin.close();
}

void getKeywords()
{
    fstream fin;
    string colname, line;
    fin.open("keyword.csv");
    while(getline(fin, line))
    {
        stringstream ss(line);
        bool flag = true;
        string start;
        while(getline(ss, colname, ','))
        {
            if(flag)
            {
                start = colname;
                flag = false;
            }
            else
                search_table[colname].push_back(start);
        }
    }
    fin.close();
}

void getImpressions()
{
    fstream fin;
    string colname, line;
    fin.open("impression.csv");
    while(getline(fin, line))
    {
        stringstream ss(line);
        string website, impression;
        getline(ss, website, ',');
        getline(ss, impression, ',');
        impressions[website] = stoi(impression);
    }
    fin.close();
}

void getCTR()
{
    fstream fin;
    string colname, line;
    fin.open("CTR.csv");
    while(getline(fin, line))
    {
        stringstream ss(line);
        string website, ctr;
        getline(ss, website, ',');
        getline(ss, ctr, ',');
        CTR[website] = stoi(ctr);
    }
    fin.close();
}

void page_rank()
{
    double size = graph.size();
    for (auto& entry : graph)
        pagerank[entry.first] = 1.0 / size;

    for (int j = 0; j < 2; ++j)
    {
        for (auto& entry : pagerank)
        {
            double temp_rank = 0.0;
            for (auto& link : incoming_links[entry.first])
                temp_rank += pagerank[link] / graph[link].size();

            entry.second = temp_rank;
        }
    }
}

bool cmp_score(pair<string,double> const& p1, pair<string,double> const& p2)
{
    return p1.second>p2.second;
}

vector<pair<string, double>> get_websites(string query)
{
    vector<pair<string, double>> final_result;
    vector<string> result=search_table[query];

    for(auto& i:result)
    {
        double score = (0.4 * pagerank[i]) + (((1.0 - ((0.1 * impressions[i]) / (1.0 + (0.1 * impressions[i])))) * pagerank[i]) + ((1.0 - ((0.1 * impressions[i]) / (1.0 + (0.1 * impressions[i])))) * CTR[i])) * 0.6;
        final_result.push_back({i, score});
    }
    sort(final_result.begin(), final_result.end(), cmp_score);
    return final_result;
}

bool print_results(vector<pair<string, double>> result)
{
    int i = 1;
    for (auto& entry : result)
    {
        cout << i << "- " << entry.first << endl;
        impressions[entry.first]++;
        i++;
    }
    int clicked;
    int clicked2;
    do
    {
        cout << "Enter the page you want to visit or 0 to return to the main page or -1 to exit the program: ";
        cin >> clicked;
        if(clicked == 0)
            return true;
        else if(clicked == -1)
            return false;
        else if((clicked > i-1) || (clicked < 1))
            cout << "Enter a valid answer!" << endl;
        else
        {
            cout << "You are now viewing: " << result[clicked-1].first <<  endl;
            CTR[result[clicked-1].first]++;
            do
            {
                cout << "1) Return to the results list." << endl << "2) Return to the main page." << endl << "3) Exit the program." << endl << "Enter your answer: ";
                cin >> clicked2;
            } while((clicked2 > 3) || (clicked2 < 1));
            switch (clicked2)
            {
                case 1:
                    return print_results(result);
                    break;
                case 2:
                    return true;
                    break;
                case 3:
                    return false;
                    break;
            }
        }
    } while((clicked > i-1) || (clicked < 1));
}

void update_registry()
{
    fstream fout;
    fout.open("CTR.csv");
    for(auto it=CTR.begin(); it!=CTR.end(); it++)
        fout << it->first<<','<<it->second<<'\n';

    fout.close();

    fout.open("impression.csv");
    for(auto it=impressions.begin(); it!=impressions.end(); it++)
        fout << it->first<<','<<it->second<<'\n';

    fout.close();
}

bool process_query(string query)
{
    bool flag;
    if(query[0] == '"')
    {
        string new_query = "";
        for(int i=1; i<query.length()-1;i++)
            new_query += query[i];

        vector<pair<string, double>> result = get_websites(new_query);
        flag = print_results(result);
    }
    else if(query.find("OR") != string::npos)
    {
        string temp;
        stringstream s(query);
        vector<pair<string, double>> result1;
        vector<pair<string, double>> result2;
        getline(s, temp, ' ');
        result1 = get_websites(temp);
        getline(s,temp,' ');
        getline(s,temp,' ');
        result2 = get_websites(temp);
        for(auto it: result1)
            if(find(result2.begin(), result2.end(), it)==result2.end())
                result2.push_back({it.first, it.second});

        flag = print_results(result2);
    }
    else if(query.find("AND") != string::npos)
    {
        string temp;
        stringstream s(query);
        vector<pair<string, double>> result1;
        vector<pair<string, double>> result2;
        vector<pair<string, double>> result;
        getline(s, temp, ' ');
        result1 = get_websites(temp);
        getline(s,temp,' ');
        getline(s,temp,' ');
        result2 = get_websites(temp);
        for(auto it: result1)
            if(find(result2.begin(), result2.end(), it)!=result2.end())
                result.push_back({it.first, it.second});

        flag = print_results(result);
    }
    else
    {
        string temp;
        stringstream s(query);
        vector<pair<string, double>> result1;
        vector<pair<string, double>> result2;
        getline(s, temp, ' ');
        result1 = get_websites(temp);
        getline(s,temp,' ');
        result2 = get_websites(temp);
        for(auto it: result1)
            if(find(result2.begin(), result2.end(), it)==result2.end())
                result2.push_back({it.first, it.second});

        flag = print_results(result2);
    }
    return flag;
}

int main()
{
    getImpressions();
    getGraph();
    getKeywords();
    getCTR();
    page_rank();
    cout << "Hello!" << endl;
    bool flag = true;
    int choice;

    do
    {
        cout << "1) New search" << endl;
        cout << "2) Exit" << endl;
        cout << "Please enter your choice : ";
        cin >> choice;

        if(choice == 1)
        {
            string query;
            cout << "Enter your search ";
            cin.ignore();
            getline(cin,query);
            flag = process_query(query);
        }
        else if(choice == 2)
            flag = false;
        else
            cout << "Enter a valid answer!" << endl;
    }while(flag);

    update_registry();
    return 0;
}