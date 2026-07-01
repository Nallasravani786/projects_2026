# Graph-Based Recommendation & Community Detection Engine

A Python graph analytics engine implementing BFS, PageRank, Union-Find-based
community detection, and friend recommendation — all from scratch, no
external graph libraries.

## Files
- `graph_engine.py` — full source (graph generation, BFS, PageRank, Union-Find
  community detection, Jaccard-similarity friend recommendation)
- `Project2_Graph_Recommendation_Engine.pdf` — write-up with verified results

## Run
```bash
python3 graph_engine.py
```

## Verified Results (from an actual run, 5,000-node / ~20,000-edge graph)
| Algorithm | Result | Time |
|---|---|---|
| BFS shortest path | 3 degrees of separation | 1.25 ms |
| PageRank (50 iterations) | Top node score 0.00478 | 179 ms |
| Community detection | 4,606 communities, largest size 27 | 33 ms |
| Friend recommendation | Best match similarity 0.053 | <1 ms |

## CV Bullet
Built a graph analytics engine from scratch implementing BFS shortest-path,
PageRank via power iteration, and Union-Find-based community detection on a
5,000-node synthetic social network; designed a "People You May Know"-style
recommendation system using Jaccard similarity, detecting 4,600+ communities
and computing PageRank convergence in under 200ms.
