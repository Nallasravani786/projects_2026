"""
Graph-Based Recommendation & Community Detection Engine
---------------------------------------------------------
Course tie-ins: Combinatorics & Graph Theory, Design and Analysis of Algorithms,
                Big Data Analysis (scalability on large graphs)

Implements (from scratch, no external graph libraries):
  1. Graph representation via adjacency list
  2. BFS shortest path / degrees-of-separation
  3. PageRank via power iteration (eigenvector-based ranking)
  4. Community detection via Union-Find (connected components on
     thresholded similarity edges) + simple label propagation
  5. Friend recommendation via Jaccard similarity of neighborhoods

Run: python3 graph_engine.py
"""

import random
import time
from collections import deque, defaultdict


# ---------------------------------------------------------------------
# 1. Graph generation (synthetic social network, Barabasi-Albert style
#    preferential attachment so it has realistic community structure)
# ---------------------------------------------------------------------
def generate_social_graph(num_nodes, edges_per_new_node=3, seed=42):
    random.seed(seed)
    adj = defaultdict(set)
    nodes = list(range(min(edges_per_new_node, num_nodes)))
    # start with a small complete graph
    for i in nodes:
        for j in nodes:
            if i != j:
                adj[i].add(j)
                adj[j].add(i)

    degree_pool = []
    for n in nodes:
        degree_pool.extend([n] * max(1, len(adj[n])))

    for new_node in range(len(nodes), num_nodes):
        targets = set()
        while len(targets) < edges_per_new_node:
            targets.add(random.choice(degree_pool))
        for t in targets:
            adj[new_node].add(t)
            adj[t].add(new_node)
            degree_pool.append(t)
            degree_pool.append(new_node)
        nodes.append(new_node)

    return adj


# ---------------------------------------------------------------------
# 2. BFS shortest path / degrees of separation
# ---------------------------------------------------------------------
def bfs_shortest_path(adj, source, target):
    if source == target:
        return [source]
    visited = {source}
    parent = {source: None}
    q = deque([source])
    while q:
        u = q.popleft()
        for v in adj[u]:
            if v not in visited:
                visited.add(v)
                parent[v] = u
                if v == target:
                    path = [v]
                    while parent[path[-1]] is not None:
                        path.append(parent[path[-1]])
                    return path[::-1]
                q.append(v)
    return None  # not connected


# ---------------------------------------------------------------------
# 3. PageRank via power iteration
# ---------------------------------------------------------------------
def pagerank(adj, num_nodes, damping=0.85, iterations=50, tol=1e-8):
    N = num_nodes
    rank = {n: 1.0 / N for n in range(N)}
    out_degree = {n: max(1, len(adj[n])) for n in range(N)}

    for it in range(iterations):
        new_rank = {n: (1 - damping) / N for n in range(N)}
        for u in range(N):
            share = rank[u] / out_degree[u]
            for v in adj[u]:
                new_rank[v] += damping * share
        diff = sum(abs(new_rank[n] - rank[n]) for n in range(N))
        rank = new_rank
        if diff < tol:
            break
    return rank


# ---------------------------------------------------------------------
# 4. Community detection via Union-Find on high-similarity edges
# ---------------------------------------------------------------------
class UnionFind:
    def __init__(self, n):
        self.parent = list(range(n))
        self.rank = [0] * n

    def find(self, x):
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]  # path compression
            x = self.parent[x]
        return x

    def union(self, x, y):
        rx, ry = self.find(x), self.find(y)
        if rx == ry:
            return
        if self.rank[rx] < self.rank[ry]:
            rx, ry = ry, rx
        self.parent[ry] = rx
        if self.rank[rx] == self.rank[ry]:
            self.rank[rx] += 1


def jaccard_similarity(adj, u, v):
    a, b = adj[u], adj[v]
    if not a and not b:
        return 0.0
    inter = len(a & b)
    union = len(a | b)
    return inter / union if union else 0.0


def detect_communities(adj, num_nodes, similarity_threshold=0.15):
    uf = UnionFind(num_nodes)
    for u in range(num_nodes):
        for v in adj[u]:
            if u < v and jaccard_similarity(adj, u, v) >= similarity_threshold:
                uf.union(u, v)
    communities = defaultdict(list)
    for n in range(num_nodes):
        communities[uf.find(n)].append(n)
    return communities


# ---------------------------------------------------------------------
# 5. Friend recommendation: rank non-neighbors by Jaccard similarity
#    (mutual-friends-based recommendation, like "People You May Know")
# ---------------------------------------------------------------------
def recommend_friends(adj, user, top_k=5):
    candidates = set()
    for friend in adj[user]:
        candidates |= adj[friend]
    candidates -= adj[user]
    candidates.discard(user)

    scored = [(jaccard_similarity(adj, user, c), c) for c in candidates]
    scored.sort(reverse=True)
    return scored[:top_k]


# ---------------------------------------------------------------------
# Main: build graph, run all algorithms, report results
# ---------------------------------------------------------------------
def main():
    NUM_NODES = 5000
    print("=== Graph-Based Recommendation & Community Detection Engine ===\n")

    t0 = time.time()
    adj = generate_social_graph(NUM_NODES, edges_per_new_node=4)
    t1 = time.time()
    total_edges = sum(len(v) for v in adj.values()) // 2
    print(f"Generated synthetic social graph: {NUM_NODES} nodes, {total_edges} edges")
    print(f"Graph generation time: {(t1 - t0)*1000:.1f} ms\n")

    # --- BFS: degrees of separation ---
    src, tgt = 0, NUM_NODES - 1
    t0 = time.time()
    path = bfs_shortest_path(adj, src, tgt)
    t1 = time.time()
    deg_sep = len(path) - 1 if path else -1
    print(f"[BFS] Shortest path from node {src} to node {tgt}: "
          f"{deg_sep} degrees of separation ({(t1-t0)*1000:.2f} ms)")

    # --- PageRank ---
    t0 = time.time()
    ranks = pagerank(adj, NUM_NODES, iterations=50)
    t1 = time.time()
    top_ranked = sorted(ranks.items(), key=lambda x: -x[1])[:5]
    print(f"\n[PageRank] Computed in {(t1-t0)*1000:.1f} ms over {NUM_NODES} nodes")
    print("Top 5 most influential nodes:")
    for node, score in top_ranked:
        print(f"   node {node}: score={score:.6f}, degree={len(adj[node])}")

    # --- Community detection ---
    t0 = time.time()
    communities = detect_communities(adj, NUM_NODES, similarity_threshold=0.03)
    t1 = time.time()
    sizes = sorted([len(members) for members in communities.values()], reverse=True)
    print(f"\n[Community Detection] Found {len(communities)} communities "
          f"in {(t1-t0)*1000:.1f} ms")
    print(f"Largest community sizes: {sizes[:10]}")

    # --- Friend recommendation ---
    test_user = 10
    recs = recommend_friends(adj, test_user, top_k=5)
    print(f"\n[Recommendation] Top friend suggestions for node {test_user}:")
    for score, candidate in recs:
        print(f"   suggest node {candidate}  (similarity={score:.3f})")


if __name__ == "__main__":
    main()
