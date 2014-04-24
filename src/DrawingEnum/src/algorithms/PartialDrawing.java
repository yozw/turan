package algorithms;

import static yz.utils.MoreMath.mod;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;

import yz.utils.CircularOrder;
import yz.utils.Pair;
import algorithms.Node.NodeType;
import algorithms.Region.Orientation;

public class PartialDrawing {

	public enum InitialRegionConfiguration { NONCROSSING, CROSSING };

	class ArcInfo {
		Edge e;

		public ArcInfo clone() {
			ArcInfo ai = new ArcInfo();
			ai.e = e;
			return ai;
		}
	}

	public static boolean SANITY_CHECKS = false;

	final ArrayList<Node> nodes;
	final Hashtable<Node, NodeType> nodeType;
	final ArrayList<Region> regions = new ArrayList<Region>();
	final Hashtable<Arc, ArcInfo> arcs;
	final HashMap<Vertex, Node> vertices;

	final Hashtable<Node, CircularOrder<Arc>> circularOrders;

	final ArrayList<Pair<Edge>> crossings;
	
	private int nextNodeNumber;

	@SuppressWarnings("unchecked")
	public PartialDrawing(PartialDrawing pd) {

		nodes = (ArrayList<Node>) pd.nodes.clone();
		nodeType = (Hashtable<Node, NodeType>) pd.nodeType.clone();
		arcs = (Hashtable<Arc, ArcInfo>) pd.arcs.clone();
		vertices = (HashMap<Vertex, Node>) pd.vertices.clone();
		circularOrders = (Hashtable<Node, CircularOrder<Arc>>) pd.circularOrders.clone();
		crossings = (ArrayList<Pair<Edge>>) pd.crossings.clone();

		for (Region r : pd.regions)
			regions.add(new Region(r));
		
		nextNodeNumber = pd.nextNodeNumber;
	}

	private void setupInitialRegionNoCrossing(VertexList V, EdgeList E, VertexList initialRegion) 
	{
		// construct regions
		Region R1 = createRegion();
		Region R2 = createRegion();

		nextNodeNumber = 1;

		R1.orientation = Orientation.CLOCKWISE;
		R2.orientation = Orientation.COUNTERCLOCKWISE;

		for (Vertex v : initialRegion) {
			Node newNode = createNode(v);
			R1.addBoundaryNode(newNode);
			R2.addBoundaryNode(newNode);

			setNodeType(newNode, NodeType.Boundary);

			vertices.put(v, newNode);
		}

		// add appropriate arcs and appropriate circular orderings
		int bdSize = initialRegion.size();
		for (int i = 0; i < bdSize; i++) {
			Node n0 = R1.getBoundaryNode(mod(i - 1, bdSize));
			Node n1 = R1.getBoundaryNode(i);
			Node n2 = R1.getBoundaryNode(mod(i + 1, bdSize));
			Vertex v1 = initialRegion.get(i);
			Vertex v2 = initialRegion.get(mod(i + 1, bdSize));
			createArc(n1, n2, E.getEdge(v1, v2));

			if (keepTrackOfCircularOrder(n1)) {
				CircularOrder<Arc> co = new CircularOrder<Arc>(new Arc[] { new Arc(n1, n0), new Arc(n1, n2) });
				circularOrders.put(n1, co);
			}
		}
	}

	private void setupInitialRegionCrossing(VertexList V, EdgeList E, VertexList initialRegion) 
	{
		assert(initialRegion.size() == 4);

		nextNodeNumber = 1;

		// construct regions
		Region R1 = createRegion();
		Region R2 = createRegion();
		Region R3 = createRegion();
		R1.orientation = Orientation.CLOCKWISE;
		R2.orientation = Orientation.CLOCKWISE;
		R3.orientation = Orientation.COUNTERCLOCKWISE;

		Node[] v = new Node[4];
		Vertex[] z = new Vertex[4];
		for (int i = 0; i < 4; i++)
			z[i] = initialRegion.get(i);

		for (int i = 0; i < 4; i++)
		{
			v[i] = createNode(z[i]);
			setNodeType(v[i], NodeType.Boundary);
			vertices.put(z[i], v[i]);
		}

		Node middleNode = createNode();
		setNodeType(middleNode, NodeType.Boundary);

		R1.addBoundaryNode(v[0]);
		R1.addBoundaryNode(v[1]);
		R1.addBoundaryNode(middleNode);

		R2.addBoundaryNode(middleNode);
		R2.addBoundaryNode(v[3]);
		R2.addBoundaryNode(v[2]);

		R3.addBoundaryNode(v[0]);
		R3.addBoundaryNode(v[1]);
		R3.addBoundaryNode(middleNode);
		R3.addBoundaryNode(v[3]);
		R3.addBoundaryNode(v[2]);
		R3.addBoundaryNode(middleNode);

		createArc(v[0], v[1], E.getEdge(z[0], z[1]));
		createArc(v[1], middleNode, E.getEdge(z[1], z[2]));
		createArc(middleNode, v[0], E.getEdge(z[0], z[3]));

		createArc(v[2], v[3], E.getEdge(z[2], z[3]));
		createArc(v[2], middleNode, E.getEdge(z[1], z[2]));
		createArc(middleNode, v[3], E.getEdge(z[0], z[3]));

		crossings.add(new Pair<Edge>(E.getEdge(z[0], z[3]), E.getEdge(z[1], z[2])));


		// set circular orders
		CircularOrder<Arc> co;
		co = new CircularOrder<Arc>(new Arc[] { new Arc(v[0], v[1]), new Arc(v[0], middleNode) });
		if (keepTrackOfCircularOrder(v[0]))
			circularOrders.put(v[0], co);

		co = new CircularOrder<Arc>(new Arc[] { new Arc(v[1], middleNode), new Arc(v[1], v[0]) });
		if (keepTrackOfCircularOrder(v[1]))
			circularOrders.put(v[1], co);

		co = new CircularOrder<Arc>(new Arc[] { new Arc(v[2], v[3]), new Arc(v[2], middleNode) });
		if (keepTrackOfCircularOrder(v[2]))
			circularOrders.put(v[2], co);

		co = new CircularOrder<Arc>(new Arc[] { new Arc(v[3], middleNode), new Arc(v[3], v[2]) });
		if (keepTrackOfCircularOrder(v[3]))
			circularOrders.put(v[3], co);
	}

	public PartialDrawing(VertexList V, EdgeList E, VertexList initialRegion, InitialRegionConfiguration config) {

		nodes = new ArrayList<Node>();
		nodeType = new Hashtable<Node, NodeType>();
		arcs = new Hashtable<Arc, ArcInfo>();
		vertices = new HashMap<Vertex, Node>();
		circularOrders = new Hashtable<Node, CircularOrder<Arc>>();
		crossings = new ArrayList<Pair<Edge>>();

		switch (config)
		{
			case NONCROSSING:
				setupInitialRegionNoCrossing(V, E, initialRegion);
				break;
			case CROSSING:
				setupInitialRegionCrossing(V, E, initialRegion);
				break;
		}
	}

	public Region[] addChord(int regionIndex, Node a, Node b, Edge e) {

		NodeType nodeTypeB = getNodeType(b);

		Region R = region(regionIndex);

		assert (getNodeType(a) == NodeType.Boundary);

		assert (R.contains(a, NodeType.Boundary) && R.contains(b, nodeTypeB));

		assert ((a.vertex == null) || e.isIncidentWith(a.vertex));
		assert ((b.vertex == null) || e.isIncidentWith(b.vertex));

		Region[] rNew = null;

		Arc newArc = null;
		CircularOrder<Arc> co;

		checkSanity();

		// a IS A BOUNDARY NODE

		switch (nodeTypeB) {
		case Boundary:
			rNew = R.split(a, b);
			newArc = createArc(a, b, e);
			regions.remove(R);
			for (int i = 0; i < rNew.length; i++)
				regions.add(rNew[i]);

			// update circular order
			if (keepTrackOfCircularOrder(b)) {
				co = circularOrders.get(b);
				assert (co != null);
				assert (co.size() > 1);
				Node pred = R.getClockwiseEarliestNeighbor(b);
				assert (pred != null);

				updateCircularOrder(b, co.insertAfter(new Arc(b, pred), newArc));
			}
			break;

		case Leaf:
			Node pb = R.getLeafParent(b);
			R.removeLeafNode(b);

			// if b is a non-terminal node, then we may as well
			// remove the leaf and add an edge between a and pb
			if (b.vertex == null) {
				removeNode(b);
				removeArc(b, pb);

				return addChord(regionIndex, a, pb, e);
			} else {
				rNew = R.split(a, pb, b);
				setNodeType(b, NodeType.Boundary);
				newArc = createArc(a, b, e);
				regions.remove(R);
				for (int i = 0; i < rNew.length; i++)
					regions.add(rNew[i]);
			}

			// update circular order
			if (keepTrackOfCircularOrder(b)) {
				co = circularOrders.get(b);
				assert (co != null);
				assert (co.size() == 1);
				updateCircularOrder(b, co.insert(newArc));
			}
			break;

		case Free:
			R.removeFreeNode(b);
			R.addLeafNode(b, a);
			setNodeType(b, NodeType.Leaf);
			newArc = createArc(a, b, e);

			// update circular order
			if (keepTrackOfCircularOrder(b))
				updateCircularOrder(b, new CircularOrder<Arc>(new Arc[] { newArc }));

			break;
		}

		// update circular order of arcs incident with a
		if (keepTrackOfCircularOrder(a)) {
			co = circularOrders.get(a);
			assert (a != null);
			assert (co.size() > 1);
			Node pred = R.getClockwiseEarliestNeighbor(a);
			updateCircularOrder(a, co.insertAfter(new Arc(a, pred), newArc));
		}

		checkSanity();

		return rNew;
	}

	public Node addCrossEdge(int regionIndex, Node u, Arc arc, Edge e) {

		checkSanity();

		Region R = region(regionIndex);

		assert (R.contains(u, getNodeType(u)));

		Region R2 = region(getOppositeRegion(regionIndex, arc));

		Edge crossingEdge = getArcEdge(arc);

		// first subdivide the arc through which we are crossing
		Node subdivisionNode = createNode();
		setNodeType(subdivisionNode, NodeType.Boundary);
		R.subdivideBoundary(arc, subdivisionNode);
		R2.subdivideBoundary(arc, subdivisionNode);
		Arc arcus = createArc(arc.u, subdivisionNode, getArcEdge(arc));
		Arc arcvs = createArc(subdivisionNode, arc.v, getArcEdge(arc));
		removeArc(arc);
		if (keepTrackOfCircularOrder(arc.u))
			updateCircularOrder(arc.u, circularOrders.get(arc.u).replace(arc, arcus));
		if (keepTrackOfCircularOrder(arc.v))
			updateCircularOrder(arc.v, circularOrders.get(arc.v).replace(arc, arcvs));

		// next, add leaf
		Node newNode = createNode();
		setNodeType(newNode, NodeType.Leaf);
		R2.addLeafNode(newNode, subdivisionNode);
		Arc leafArc = createArc(subdivisionNode, newNode, e);
		if (keepTrackOfCircularOrder(newNode))
			updateCircularOrder(newNode, new CircularOrder<Arc>(new Arc[] { leafArc }));

		// NOTE: We should update the circular order of the new 'subdivision
		// node' PROPERLY as well, but we won't need it, so we will not do so
		if (keepTrackOfCircularOrder(subdivisionNode))
			updateCircularOrder(subdivisionNode, new CircularOrder<Arc>(new Arc[] { arcus, arcvs, leafArc }));

		// now, depending on the type of node u, update region 1
		Region[] newR = null;

		CircularOrder<Arc> co;

		crossings.add(new Pair<Edge>(e, crossingEdge));

		Arc newArc = null;

		switch (getNodeType(u)) {
		case Boundary:
			newR = R.split(u, subdivisionNode);
			regions.remove(R);
			for (int i = 0; i < newR.length; i++)
				regions.add(newR[i]);
			newArc = createArc(u, subdivisionNode, e);

			if (keepTrackOfCircularOrder(u)) {
				co = circularOrders.get(u);
				assert ((co != null) && (co.size() > 1));
				Node pred = R.getClockwiseEarliestNeighbor(u);
				updateCircularOrder(u, co.insertAfter(new Arc(u, pred), newArc));
			}
			break;

		case Leaf:
			Node pu = R.getLeafParent(u);
			R.removeLeafNode(u);

			// if b is a non-terminal node, then we may as well
			// remove the leaf and add an edge between a and pb
			if (u.vertex == null) {
				removeArc(u, pu);
				removeNode(u);
				circularOrders.remove(u);
				addChord(regionIndex, subdivisionNode, pu, e);
				return newNode;
			} else {
				newR = R.split(pu, subdivisionNode, u);
				setNodeType(u, NodeType.Boundary);
				newArc = createArc(u, subdivisionNode, e);
				if (keepTrackOfCircularOrder(u)) {
					co = circularOrders.get(u);
					assert ((co != null) && (co.size() == 1));
					updateCircularOrder(u, co.insert(newArc));
				}
				regions.remove(R);
				for (int i = 0; i < newR.length; i++)
					regions.add(newR[i]);
			}

			break;

		case Free:
			R.removeFreeNode(u);
			R.addLeafNode(u, subdivisionNode);
			setNodeType(u, NodeType.Leaf);
			newArc = createArc(u, subdivisionNode, e);

			if (keepTrackOfCircularOrder(u))
				updateCircularOrder(u, new CircularOrder<Arc>(new Arc[] { newArc }));
			break;
		}

		assert (newArc != null);

		if (keepTrackOfCircularOrder(subdivisionNode))
			updateCircularOrder(subdivisionNode, circularOrders.get(subdivisionNode).insert(newArc));

		checkSanity();

		return newNode;
	}

	public Node addFreeNode(Vertex v, int regionIndex) {
		Region R = regions.get(regionIndex);

		Node newNode = createNode(v);
		R.addFreeNode(newNode);
		setNodeType(newNode, NodeType.Free);
		vertices.put(v, newNode);

		return newNode;
	}

	public void checkSanity() {
		if (!SANITY_CHECKS)
			return;

		try {

			for (Region R : regions) {
				for (Node n : R.getBoundaryNodes()) {
					if (getNodeType(n) != NodeType.Boundary)
						throw new Exception("Boundary node " + n + " has unexpected value for nodetype: " + getNodeType(n));
					if (!nodes.contains(n))
						throw new Exception("Boundary node " + n + " is not included in nodes set");
				}
				for (Node n : R.getLeafNodes()) {
					if (getNodeType(n) != NodeType.Leaf)
						throw new Exception("Leaf node " + n + " has unexpected value for nodetype: " + getNodeType(n));
					if (!nodes.contains(n))
						throw new Exception("Leaf node " + n + " is not included in nodes set");
				}
				for (Node n : R.getFreeNodes()) {
					if (getNodeType(n) != NodeType.Free)
						throw new Exception("Free node " + n + " has unexpected value for nodetype: " + getNodeType(n));
					if (!nodes.contains(n))
						throw new Exception("Free node " + n + " is not included in nodes set");
				}
			}

			for (Node n : nodes) {
				// check that every leaf node and every free node is in exactly
				// one region
				NodeType nt = getNodeType(n);
				int inRegionCount = 0;
				for (Region R : regions)
					if (R.contains(n, nt))
						inRegionCount++;

				switch (nt) {
				case Boundary:
					break;
				case Free:
					if (inRegionCount != 1)
						throw new Exception("Free node " + n + " is supposed to be in a unique region, but instead is part of "
								+ inRegionCount + "regions.");
					break;
				case Leaf:
					if (inRegionCount != 1)
						throw new Exception("Leaf node " + n + " is supposed to be in a unique region, but instead is part of "
								+ inRegionCount + "regions.");
					break;
				}

			}
			for (Arc arc : arcs.keySet()) {
				if (!nodes.contains(arc.u))
					throw new Exception("Arc " + arc + " uses node " + arc.u + " which is not in the node set");
				if (!nodes.contains(arc.v))
					throw new Exception("Arc " + arc + " uses node " + arc.v + " which is not in the node set");

			}

			// check circular orders

			for (Node n : circularOrders.keySet()) {
				CircularOrder<Arc> co = circularOrders.get(n);
				for (Arc a : co)
					if (!a.isIncidentWith(n))
						throw new Exception("The circular order " + co + " associated with " + n + " contains the arc " + a
								+ " which is not incident with " + n + ".");
			}

		} catch (Exception E) {
			SystemErrPrinter sep = new SystemErrPrinter();
			sep.println("**********************");
			sep.println("SANITY CHECK EXCEPTION");
			sep.println("**********************");
			sep.println("The following partial drawing is invalid:");
			print(sep);
			sep.println("Message: " + E.getMessage());
			sep.println("An exception is being thrown");
			throw new Error("Sanity check failed");
		}
	}

	public boolean containsRegion(Region R) {
		return regions.contains(R);
	}

	public boolean containsVertex(Vertex v) {
		return vertices.containsKey(v);
	}

	public PartialDrawing clone() {
		PartialDrawing newPd = new PartialDrawing(this);
		newPd.checkSanity();
		return newPd;
	}

	public Arc createArc(Node u, Node v, Edge e) {
		assert(e != null);
		assert(u != null);
		assert(v != null);

		Arc a = new Arc(u, v);
		ArcInfo ai = new ArcInfo();
		ai.e = e;
		arcs.put(a, ai);

		return a;
	}

	private Node createNode() {
		Node newNode = new Node(nextNodeNumber++);
		nodes.add(newNode);

		if (keepTrackOfCircularOrder(newNode))
			circularOrders.put(newNode, new CircularOrder<Arc>());
		return newNode;
	}

	private Node createNode(Vertex v) {
		Node newNode = new Node(v);
		nodes.add(newNode);
		vertices.put(v, newNode);

		if (keepTrackOfCircularOrder(newNode))
			circularOrders.put(newNode, new CircularOrder<Arc>());
		return newNode;
	}

	private Region createRegion() {
		Region newRegion = new Region();
		regions.add(newRegion);
		return newRegion;
	}

	public Node findVertex(Vertex v) {
		for (Node n : nodes)
			if (n.vertex == v)
				return n;
		return null;
	}

	public Edge getArcEdge(Arc arc) {
		assert (arcs.containsKey(arc));
		return arcs.get(arc).e;
	}

	public Collection<Arc> getArcs() {
		return arcs.keySet();
	}

	public int[] getCommonRegions(Node a, Node b) {
		ArrayList<Integer> commonRegions = new ArrayList<Integer>();

		NodeType ta = getNodeType(a);
		NodeType tb = getNodeType(b);

		for (int i = 0; i < regions.size(); i++) {
			Region R = regions.get(i);
			if (R.contains(a, ta) && R.contains(b, tb))
				commonRegions.add(i);
		}

		return toIntArray(commonRegions);
	}

	public int getCrossingCount() {
		return crossings.size();
	}

	public Collection<Pair<Edge>> getCrossings() {
		return crossings;
	}

	public Collection<Node> getNodes() {
		return nodes;
	}

	public NodeType getNodeType(Node n) {
		return nodeType.get(n);
	}

	public int getOppositeRegion(int regionIndex, Arc arc) {
		for (int i = 0; i < regions.size(); i++) {
			if (i == regionIndex)
				continue;
			Region R = region(i);
			
			if (R.boundaryContainsArc(arc))
				return i;
		}
		return -1;
	}

	public List<Integer> getRegions(Node z) {
		List<Integer> list = new ArrayList<Integer>();
		NodeType t = getNodeType(z);
		for (int i = 0; i < regions.size(); i++)
			if (regions.get(i).contains(z, t))
				list.add(i);

		return list;
	}

	public void print(StreamPrinter out) {
		out.print("* " + regions.size() + " regions: ");
		for (Region R : regions)
			out.print(R + " ");
		out.println();
		out.print("* " + nodes.size() + " nodes: ");
		for (Node node : nodes)
			out.print(node + " ");
		out.println();
		out.print("* " + arcs.size() + " arcs: ");
		for (Arc arc : arcs.keySet())
			out.print(arc + " ");
		out.println();
		out.print("* " + crossings + " crossings");
		out.println();
	}

	public Region region(int regionIndex) {
		return regions.get(regionIndex);
	}

	public int regionCount() {
		return regions.size();
	}

	public void removeArc(Arc arc) {
		arcs.remove(arc);
	}

	public void removeArc(Node u, Node v) {
		arcs.remove(new Arc(u, v));
	}

	public void removeNode(Node a) {
		nodes.remove(a);
		circularOrders.remove(a);
	}

	private void setNodeType(Node n, NodeType t) {
		nodeType.put(n, t);
	}

	public int[] toIntArray(Collection<Integer> coll) {
		int[] array = new int[coll.size()];
		int index = 0;
		for (Integer i : coll)
			array[index++] = i;
		return array;
	}

	public CircularOrder<Arc> getCircularOrder(Node n) {
		return circularOrders.get(n);
	}

	public CircularOrder<Vertex> getCircularOrder(Vertex v) {

		Node v_node = vertices.get(v);
		if (v_node == null)
			return null;
		
		CircularOrder<Arc> co = circularOrders.get(v_node);

		if (co == null)
			return null;

		if (v_node == null)
			return null;

		Arc[] arcOrder = new Arc[co.size()];
		co.toArray(arcOrder);

		Vertex[] order = new Vertex[arcOrder.length];

		for (int i = 0; i < arcOrder.length; i++) {
			ArcInfo ai = arcs.get(arcOrder[i]);
			assert (ai != null);
			assert (ai.e.isIncidentWith(v));
			order[i] = ai.e.opposite(v);
		}

		return new CircularOrder<Vertex>(order);
	}

	private void updateCircularOrder(Node u, CircularOrder<Arc> newOrder) {
		if (SANITY_CHECKS)
			for (Arc a : newOrder)
				if (!a.isIncidentWith(u))
					throw new java.lang.Error("Cannot update circular order: the circular order " + newOrder + " associated with " + u
							+ " contains the arc " + a + " which is not incident with " + u + ".");
		circularOrders.put(u, newOrder);
	}

	private boolean keepTrackOfCircularOrder(Node u) {
		if (!DrawingEnumeration.KEEP_CIRCULAR_ORDERS)
			return false;
		return u.vertex != null;
	}

}
