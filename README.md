# Search Engine

A C++ search engine that ranks web pages using a combination of **PageRank** over a web graph and historical **click-through rate (CTR)**.

## How it works

- Builds a web graph from `webGraph.csv`
- Computes PageRank scores for each page
- Blends PageRank with per-keyword CTR (`CTR.csv`, `keyword.csv`, `impression.csv`) to rank results
- Returns the ranked pages for a query

## Build & run

```sh
g++ main.cpp -o search
./search
```

> Coursework project exploring information retrieval and ranking.
