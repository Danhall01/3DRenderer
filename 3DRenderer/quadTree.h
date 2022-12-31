#pragma once
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <memory>

#define NODE_MAX_ELEMENTS 5

template <typename T>
class quadTree
{
private: // Misc
	struct Node
	{
		T* element[NODE_MAX_ELEMENTS];
		dx::BoundingBox elementBox[NODE_MAX_ELEMENTS];

		dx::BoundingBox nodeBox;
		Node* children[4];
	};

public: // Methods
	quadTree();
	~quadTree();
	void Init(float maxHeight, float minHeight, float multiplier);

	void PrintTree() const;
	void AddElement(T* elementAddress, const dx::BoundingBox& boundingBox);
	std::vector<const T*> CheckTree(const dx::BoundingFrustum& frustum) const;
private:
	void PrintTree(Node* nodeToProcess) const;
	void AddToNode(T* elementAddress, const dx::BoundingBox& boundingBox, Node* nodeToProcess);
	void CheckNode(const dx::BoundingFrustum& frustum, Node* node, std::vector<const T*>& out_validElements) const;

	void FreeNode(Node* node);

	bool isLeaf(Node* nodeToProcess) const;
	bool isFull(Node* nodeToProcess, int& out_index) const;
	void SplitNode(Node* nodeToProcess);

public: // Variables
private:
	Node* m_root;
	float m_maxHeight;
	float m_minHeight;
};



template<typename T>
inline quadTree<T>::quadTree() : m_minHeight(-20.0f), m_maxHeight(20.0f)
{
	m_root = nullptr;
}
template<typename T>
inline quadTree<T>::~quadTree()
{
	//Recursively go through the tree to free every node
	FreeNode(m_root);
}

// ################################### Definitions ###################################
template<typename T>
inline void quadTree<T>::Init(float maxHeight, float minHeight, float multiplier)
{
	m_maxHeight = maxHeight;
	m_minHeight = minHeight;

	// Creates the root bounding box
	// from (-1, min_height, -1) to (1, max_height, 1)

	dx::XMVECTOR topleft = { -1.0f * multiplier, m_minHeight, -1.0f * multiplier };
	dx::XMVECTOR bottomRigh = { 1.0f * multiplier, m_maxHeight, 1.0f * multiplier };
	dx::BoundingBox rootBox;
	dx::BoundingBox::CreateFromPoints(rootBox, topleft, bottomRigh);


	m_root = (Node*)malloc(sizeof(Node));
	if(!m_root)
	{	
		exit(1);
	}

	for (int i = 0; i < 4; i++)
	{
		m_root->children[i] = nullptr;
	}
	for (int i = 0; i < NODE_MAX_ELEMENTS; ++i)
	{
		m_root->element[i] = nullptr;
		m_root->elementBox[i] = {};
	}


	m_root->nodeBox = rootBox;
}

template<typename T>
inline void quadTree<T>::FreeNode(Node* node)
{
	if (node == nullptr)
	{
		return;
	}
	if (isLeaf(node))
	{
		free(node);
		node = nullptr;
		return;
	}
	// This is an initialized parent
	for (int i = 0; i < 4; ++i)
	{
		FreeNode(node->children[i]);
	}
}




template<typename T>
inline void quadTree<T>::PrintTree() const
{
	PrintTree(m_root);
}
template<typename T>
inline void quadTree<T>::PrintTree(Node* nodeToProcess) const
{
	if (isLeaf(nodeToProcess))
	{
		// Print data

	}
	else
	{
		// Recursively call every child
		PrintTree(nodeToProcess->children[0]);
		PrintTree(nodeToProcess->children[1]);
		PrintTree(nodeToProcess->children[2]);
		PrintTree(nodeToProcess->children[3]);
	}
}




template<typename T>
inline void quadTree<T>::AddElement(T* elementAddress, const dx::BoundingBox& boundingBox)
{
	AddToNode(elementAddress, boundingBox, m_root );
}
template<typename T>
inline void quadTree<T>::AddToNode(T* elementAddress, const dx::BoundingBox& boundingBox, Node* nodeToProcess)
{
	bool collision = nodeToProcess->nodeBox.Intersects(boundingBox);

	if (!collision)
		return;

	if (isLeaf(nodeToProcess))
	{
		int nodeElements = 0;
		if (isFull(nodeToProcess, nodeElements))
		{
			// Make this node a parent
			SplitNode(nodeToProcess);

			// Add the current nodes element to a child node
			for (int elementIndex = 0; elementIndex < NODE_MAX_ELEMENTS; elementIndex++)
			{
				for (int childIndex = 0; childIndex < 4; ++childIndex)
				{
					AddToNode(nodeToProcess->element[elementIndex], nodeToProcess->elementBox[elementIndex], nodeToProcess->children[childIndex]);
				}

			}
			// Clear node
			for (int i = 0; i < NODE_MAX_ELEMENTS; ++i)
			{
				nodeToProcess->element[i] = nullptr;
				nodeToProcess->elementBox[i] = {};
			}
		}
		else
		{
			// Add element to this node
			nodeToProcess->element[nodeElements] = elementAddress;
			nodeToProcess->elementBox[nodeElements] = boundingBox;
			return;
		}
	}

	// Node is either parent or was made into one, pass along to next nodes
	AddToNode(elementAddress, boundingBox, nodeToProcess->children[0]);
	AddToNode(elementAddress, boundingBox, nodeToProcess->children[1]);
	AddToNode(elementAddress, boundingBox, nodeToProcess->children[2]);
	AddToNode(elementAddress, boundingBox, nodeToProcess->children[3]);
	return;
}




template<typename T>
inline std::vector<const T*> quadTree<T>::CheckTree(const dx::BoundingFrustum& frustum) const
{
	std::vector<const T*> toReturn;
	CheckNode(frustum, m_root, toReturn);
	return toReturn;
}
template<typename T>
inline void quadTree<T>::CheckNode(const dx::BoundingFrustum& frustum, Node* node, std::vector<const T*>& out_validElements) const
{
	//Check if node bounding volume is in the frustum
	bool collision = frustum.Contains(node->nodeBox);
	if (!collision)
	{
		return;
	}

	if (isLeaf(node))
	{
		for (int i = 0; i < NODE_MAX_ELEMENTS; ++i)
		{
			if (node->element[i] == nullptr)
			{
				break;
			}

			// Check if element bounding volume is in the frustum
			collision = frustum.Contains(node->elementBox[i]);
			if(!collision)
			{
				continue;
			}

			//Check if element is already in return vector
			bool found = false;
			T* nodeValue = node->element[i];
			for (auto& element : out_validElements)
			{
				if (element == nodeValue)
				{
					found = true;
					break;
				}
			}

			//add unique element to vector
			if (!found)
			{
				out_validElements.push_back(nodeValue);
			}

		}
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			CheckNode(frustum, node->children[i], out_validElements);
		}
	}

}






// Helper functions

template<typename T>
inline bool quadTree<T>::isLeaf(Node* nodeToProcess) const
{
	return nodeToProcess->children[0] == nullptr;
}

template<typename T>
inline bool quadTree<T>::isFull(Node* nodeToProcess, int& out_index) const
{
	out_index = 0;
	for (int i = 0; i < NODE_MAX_ELEMENTS; ++i)
	{
		if (nodeToProcess->element[i] != nullptr)
		{
			++out_index;
		}
		else
		{
			break;
		}
	}
	return out_index == (NODE_MAX_ELEMENTS);
}

template<typename T>
inline void quadTree<T>::SplitNode(Node* nodeToProcess)
{
	dx::BoundingBox tempBox;
	dx::XMFLOAT3 C = nodeToProcess->nodeBox.Center;
	dx::XMFLOAT3 R = nodeToProcess->nodeBox.Extents;


	// new boundingbox topleft will be,     topLeftBack ( )  and bottomRightForward ( )
	nodeToProcess->children[0] = (Node*)malloc(sizeof(Node));
	if(nodeToProcess->children[0])
	{
		tempBox.CreateFromPoints(nodeToProcess->children[0]->nodeBox, {C.x - R.x, m_minHeight, C.z + R.z}, {C.x, m_maxHeight,	C.z});
	}

	// new boundingbox topright will be,    topLeftBack ( )  and bottomRightForward ( )
	nodeToProcess->children[1] = (Node*)malloc(sizeof(Node));
	if (nodeToProcess->children[1]) 
	{
		tempBox.CreateFromPoints(nodeToProcess->children[1]->nodeBox, {C.x, m_minHeight, C.z + R.z}, {C.x + R.x, m_maxHeight, C.z});
	}

	// new boundingbox bottomleft will be,  topLeftBack ( )  and bottomRightForward ( )
	nodeToProcess->children[2] = (Node*)malloc(sizeof(Node));
	if (nodeToProcess->children[2])
	{
		tempBox.CreateFromPoints(nodeToProcess->children[2]->nodeBox, { C.x - R.x, m_minHeight, C.z }, { C.x, m_maxHeight, C.z - R.z });
	}

	// new boundingbox bottomright will be, topLeftBack ( cx, cy, 0)  and bottomRightForward ( cx + r, cy - r, 1)
	nodeToProcess->children[3] = (Node*)malloc(sizeof(Node));
	if (nodeToProcess->children[3])
	{
		tempBox.CreateFromPoints(nodeToProcess->children[3]->nodeBox, { C.x, m_minHeight,C.z }, { C.x + R.x, m_maxHeight, C.z - R.z });
	}



	// Created children for the node, sanitise the new nodes for future use
	for (int i = 0; i < 4; ++i)
	{
		nodeToProcess->children[i]->children[0] = nullptr;
		nodeToProcess->children[i]->children[1] = nullptr;
		nodeToProcess->children[i]->children[2] = nullptr;
		nodeToProcess->children[i]->children[3] = nullptr;
	}
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < NODE_MAX_ELEMENTS; ++j)
		{
			nodeToProcess->children[i]->element[j] = nullptr;
			nodeToProcess->children[i]->elementBox[j] = {};
		}
	}
}