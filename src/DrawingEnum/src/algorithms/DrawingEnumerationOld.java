package algorithms;

public class DrawingEnumerationOld {

	/*
	 * private final ArrayList<Vertex> V = new ArrayList<Vertex>(); private
	 * final ArrayList<Edge> E = new ArrayList<Edge>();
	 * 
	 * private final ArrayList<Node> Nodes = new ArrayList<Node>();
	 * 
	 * private final ConsoleOutput out = new ConsoleOutput(); public int
	 * OUTPUT_LEVEL = 5;
	 * 
	 * class PathState { int crossings = 0; }
	 * 
	 * public DrawingEnumeration(Collection<Vertex> V, Collection<Edge> E) {
	 * this.V.addAll(V); this.E.addAll(E); }
	 * 
	 * public void completeDrawing(ArrayList<Vertex> initialRegion) throws
	 * Exception {
	 * 
	 * for (Vertex v : V) v.marked = false;
	 * 
	 * Region R1 = new Region(); Region R2 = new Region();
	 * 
	 * for (Vertex v : initialRegion) { Node newNode = new Node(v);
	 * R1.boundary.add(newNode); R2.boundary.add(newNode); newNode.type =
	 * NodeType.Boundary; newNode.regions.add(R1); newNode.regions.add(R2);
	 * Nodes.add(newNode); v.marked = true; }
	 * 
	 * Stack<Vertex> remainingVertices = new Stack<Vertex>(); for (Vertex v : V)
	 * if (!v.marked) remainingVertices.push(v);
	 * 
	 * Stack<Edge> remainingEdges = new Stack<Edge>();
	 * 
	 * ArrayList<Region> regions = new ArrayList<Region>(); regions.add(R1);
	 * regions.add(R2);
	 * 
	 * completeDrawing(regions, remainingVertices, remainingEdges); }
	 * 
	 * private void printStatus(ArrayList<Region> regions, Stack<Vertex>
	 * remainingVertices, Stack<Edge> remainingEdges) { out.print("* " +
	 * regions.size() + " regions: "); for (Region R : regions) out.print(R +
	 * " "); out.println(); out.print("* " + remainingVertices.size() +
	 * " unmarked vertices: "); for (Vertex v : remainingVertices) out.print(v +
	 * " "); out.println(); out.print("* " + remainingEdges.size() +
	 * " remaining edges: "); for (Edge e : remainingEdges) out.print(e + " ");
	 * out.println();
	 * 
	 * }
	 * 
	 * private void completeDrawing(ArrayList<Region> regions, Stack<Vertex>
	 * remainingVertices, Stack<Edge> remainingEdges) throws Exception {
	 * 
	 * checkSanity(regions, remainingVertices, remainingEdges);
	 * 
	 * out.increaseIndent(2);
	 * 
	 * try {
	 * 
	 * // print some information if (OUTPUT_LEVEL >= 5) { out.println();
	 * out.println("completeDrawing called with the following input:");
	 * printStatus(regions, remainingVertices, remainingEdges); }
	 * 
	 * // CASE 1: We run out of edges if (remainingEdges.size() == 0) { if
	 * (remainingVertices.size() == 0) { if (OUTPUT_LEVEL >= 5) out
	 * .println("Ran out of edges and vertices, so we are done!"); return; }
	 * 
	 * Vertex newVertex = remainingVertices.pop(); if (OUTPUT_LEVEL >= 10)
	 * System.out.println(">> Adding a new vertex "); Node newNode = new
	 * Node(newVertex); Nodes.add(newNode); newVertex.marked = true;
	 * 
	 * newNode.type = NodeType.Free; for (Region R : regions) {
	 * R.freeNodes.add(newNode); newNode.region = R;
	 * 
	 * Stack<Edge> newEdgeStack = new Stack<Edge>(); for (Edge e : E) { if
	 * (!e.isIncidentWith(newVertex)) continue; if
	 * (e.opposite(newVertex).marked) newEdgeStack.push(e); }
	 * 
	 * out.println("Added free node " + newNode + " in region " + R);
	 * completeDrawing(regions, remainingVertices, newEdgeStack);
	 * R.freeNodes.remove(newNode); } Nodes.remove(newNode); newVertex.marked =
	 * false; remainingVertices.push(newVertex); return; }
	 * 
	 * // CASE 2: We have an edge to add Edge e = remainingEdges.pop();
	 * 
	 * try { Node nu = null, nv = null; for (Node n : Nodes) { if (n.vertex ==
	 * e.u) nu = n; else if (n.vertex == e.v) nv = n;
	 * 
	 * if ((nu != null) && (nv != null)) break; }
	 * 
	 * assert (nu != null); assert (nv != null);
	 * 
	 * completePath(regions, nu, nv, e, remainingVertices, remainingEdges, new
	 * PathState()); } finally { remainingEdges.push(e); }
	 * 
	 * } finally { out.increaseIndent(-2); }
	 * 
	 * }
	 * 
	 * 
	 * private void completePath(ArrayList<Region> regions, Node a, Node b, Edge
	 * e, Stack<Vertex> remainingVertices, Stack<Edge> remainingEdges, PathState
	 * state) throws Exception { // this function requires that at least one of
	 * a, b is on the boundary // of some region assert((a.type ==
	 * NodeType.Boundary) || (b.type == NodeType.Boundary));
	 * 
	 * if (b.type == NodeType.Boundary) { Node t = a; a = b; b = t; }
	 * 
	 * // so from now on, we may assume that a is a boundary node if
	 * (OUTPUT_LEVEL >= 5) { out.println();
	 * out.println("completePath called for nodes " + a + " and " + b);
	 * printStatus(regions, remainingVertices, remainingEdges); out.println("* "
	 * + a + " is of type " + a.type); out.println("* " + b + " is of type " +
	 * b.type); out.println("* current number of crossings: " +
	 * state.crossings); }
	 * 
	 * checkSanity(regions, remainingVertices, remainingEdges);
	 * 
	 * int numberOfRecursions = 0;
	 * 
	 * if (b.type == NodeType.Boundary) { // ---- a and b are both boundary
	 * nodes ----
	 * 
	 * // 1) for each region that has both a and b on the boundary, // try
	 * adding a straight curve between them HashSet<Region> commonRegions = new
	 * HashSet<Region>(); commonRegions.addAll(a.regions);
	 * commonRegions.removeAll(a.regions);
	 * 
	 * for (Region R: commonRegions) { ArrayList<Region> newRegions = new
	 * ArrayList<Region>(); for (Region Rp: regions) if (Rp != b.region)
	 * newRegions.add(Rp);
	 * 
	 * Region[] rNew = R.split(a, b);
	 * 
	 * for (int i = 0; i < rNew.length; i++) { for (Node n: rNew[i].boundary)
	 * n.regions.add(rNew[i]); newRegions.add(rNew[i]); }
	 * 
	 * out.println("Added chord between " + a + " and " + b);
	 * numberOfRecursions++; completeDrawing(newRegions, remainingVertices,
	 * remainingEdges);
	 * 
	 * for (int i = 0; i < rNew.length; i++) { for (Node n: rNew[i].boundary)
	 * n.regions.remove(rNew[i]); } }
	 * 
	 * // 2) for each region that has b on the boundary, // make a crossing
	 * through the interior of the region // going through each possible choice
	 * of arc into // the `other side' and recurse }
	 * 
	 * if (b.type == NodeType.Leaf) { // ---- a is a boundary node and b is a
	 * leaf ----
	 * 
	 * // 1) if b is a leaf inside a region that has a on the // boundary of,
	 * try adding a straight curve between them if
	 * (a.regions.contains(b.region)) { ArrayList<Region> newRegions = new
	 * ArrayList<Region>(); for (Region R: regions) if (R != b.region)
	 * newRegions.add(R);
	 * 
	 * // split region b.region into two parts Region R1 = new Region(); Region
	 * R2 = new Region(); Node pb = b.region.leafNodes.get(b);
	 * 
	 * // find a and pb on the boundary int stage = 0; int bdsize =
	 * b.region.boundary.size(); for (int i = 0; i < 2 bdsize; i++) { Node ni =
	 * b.region.boundary.get(i % bdsize); if (( ni == a) || (ni == pb)) { if
	 * (stage == 1) R1.boundary.add(ni); else if (stage == 2)
	 * R2.boundary.add(ni); stage++; } if (stage == 1) R1.boundary.add(ni); else
	 * if (stage == 2) R2.boundary.add(ni); } R1.boundary.add(b);
	 * R2.boundary.add(b); for (Node n: R1.boundary) n.regions.add(R1); for
	 * (Node n: R2.boundary) n.regions.add(R2);
	 * 
	 * b.type = NodeType.Boundary;
	 * 
	 * newRegions.add(R1); newRegions.add(R2);
	 * 
	 * numberOfRecursions++; out.println("Made the leaf node " + b +
	 * " adjacent to " + a + "."); completeDrawing(newRegions,
	 * remainingVertices, remainingEdges);
	 * 
	 * for (Node n: R1.boundary) n.regions.remove(R1); for (Node n: R2.boundary)
	 * n.regions.remove(R2);
	 * 
	 * b.type = NodeType.Leaf; }
	 * 
	 * // 2) for each edge on the boundary of the region that b // is in make a
	 * crossing through that edge into // the `other side' and recurse }
	 * 
	 * if (b.type == NodeType.Free) { // ---- a is a boundary node and b is a
	 * free node ----
	 * 
	 * // 1) if b is inside a region that has a on the // boundary of, try
	 * adding a straight curve between them, // making b a leaf of the region if
	 * (a.regions.contains(b.region)) { out.println(a + " and " + b +
	 * " share a region"); b.region.freeNodes.remove(b);
	 * b.region.leafNodes.put(b, a);
	 * 
	 * b.type = NodeType.Leaf;
	 * 
	 * out.println("Made the free vertex " + b + " adjacent to " + a);
	 * numberOfRecursions++; completeDrawing(regions, remainingVertices,
	 * remainingEdges);
	 * 
	 * b.region.leafNodes.remove(b); b.region.freeNodes.add(b); b.type =
	 * NodeType.Free; }
	 * 
	 * 
	 * // 2) for each edge on the boundary of the region that b // is in make a
	 * crossing through that edge into // the `other side' and recurse Node
	 * newBoundaryNode = new Node(); Node newLeafNode = new Node();
	 * newBoundaryNode.type = NodeType.Boundary; newLeafNode.type =
	 * NodeType.Leaf; Nodes.add(newBoundaryNode); Nodes.add(newLeafNode); b.type
	 * = NodeType.Leaf;
	 * 
	 * for (Arc arc: b.region.getBoundaryArcs()) {
	 * out.println("Adding crossing from " + a + " to " + b + " through arc " +
	 * arc);
	 * 
	 * HashSet<Region> twoRegions = new HashSet<Region>();
	 * 
	 * for (Region r: regions) for (Arc r_arc: r.getBoundaryArcs()) if
	 * (r_arc.equals(arc)) { twoRegions.add(r); break; }
	 * 
	 * if (twoRegions.size() != 2) { for (Region r: twoRegions) out.println(r);
	 * throw new
	 * SanityException("Expected exactly two regions to be bounded by edge " +
	 * arc + ". Found " + twoRegions.size() + " regions instead!"); }
	 * 
	 * Region innerRegion = b.region, outerRegion = null; for (Region Rp:
	 * twoRegions) if (Rp != b.region) outerRegion = Rp;
	 * 
	 * Region innerRegion_copy = b.region.copy(); Region outerRegion_copy =
	 * outerRegion.copy();
	 * 
	 * innerRegion.subdivideBoundary(arc, newBoundaryNode);
	 * outerRegion.subdivideBoundary(arc, newBoundaryNode);
	 * 
	 * newBoundaryNode.regions.clear();
	 * newBoundaryNode.regions.add(innerRegion);
	 * newBoundaryNode.regions.add(outerRegion);
	 * 
	 * innerRegion.leafNodes.put(b, newBoundaryNode);
	 * outerRegion.leafNodes.put(newLeafNode, newBoundaryNode);
	 * innerRegion.freeNodes.remove(b); newLeafNode.region = outerRegion;
	 * 
	 * state.crossings++; numberOfRecursions++; completePath(regions, a,
	 * newLeafNode, e, remainingVertices, remainingEdges, state);
	 * state.crossings--;
	 * 
	 * innerRegion.copyFrom(innerRegion_copy);
	 * outerRegion.copyFrom(outerRegion_copy); } b.type = NodeType.Free;
	 * Nodes.remove(newBoundaryNode); Nodes.remove(newLeafNode); }
	 * 
	 * if (numberOfRecursions == 0) out.println("Dead end"); }
	 * 
	 * private void checkSanity(ArrayList<Region> regions, Stack<Vertex>
	 * remainingVertices, Stack<Edge> remainingEdges) throws Exception { for
	 * (Region R: regions) { for (Node n: R.boundary) { if (n.type !=
	 * NodeType.Boundary) throw new
	 * SanityException("Type field of boundary node " + n +
	 * " has unexpected value " + n.type); if (!n.regions.contains(R)) throw new
	 * SanityException("Regions field of boundary node " + n +
	 * " does not contain region " + R); } for (Node n: R.freeNodes) if (n.type
	 * != NodeType.Free) throw new SanityException("Type field of free node " +
	 * n + " has unexpected value " + n.type); for (Node n:
	 * R.leafNodes.keySet()) if (n.type != NodeType.Leaf) throw new
	 * SanityException("Type field of leaf node " + n + " has unexpected value "
	 * + n.type); }
	 * 
	 * for (Node n: Nodes) { switch (n.type) { case Boundary: for (Region r:
	 * n.regions) { boolean found = false; for (int i = 0; (i <
	 * r.boundary.size()) && (!found); i++) if (r.boundary.get(i) == n) found =
	 * true; if (!found) throw new
	 * SanityException("Regions field of boundary node " + n +
	 * " unexpectedly refers to region " + r); } break; case Leaf: if (n.region
	 * == null) throw new SanityException("Leaf node " + n +
	 * " has empty region field"); if (!n.region.leafNodes.containsKey(n)) throw
	 * new SanityException("Region field of leaf node " + n +
	 * " unexpectedly refect to " + n.region); break; case Free: if (n.region ==
	 * null) throw new SanityException("Free node " + n +
	 * " has empty region field"); if (!n.region.freeNodes.contains(n)) throw
	 * new SanityException("Region field of free node " + n +
	 * " unexpectedly refect to " + n.region); break; } } }
	 */

}
