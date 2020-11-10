#ifndef __MYOCTANTCLASS_H_
#define __MYOCTANTCLASS_H_

#include  "MyEntityManager.h"

namespace Simplex
{

// System Class
class MyOctant
{
	static uint m_uOctantCount; // will store the number of octants instantiated
	static uint m_uMaxLevel; // will store the maximum level an octant can go
	static uint m_uIdealEntityCount; // will tell how many ideal Entities this object will contain

	uint m_uID = 0; // will store the current ID for this octant
	uint m_uLevel = 0; // will store the current level of the octant
	uint m_uChildren = 0; // number of children on the octant (either 0 or 8)

	float m_fSize = 0.0f; // size of the octant

	MeshManager* m_pMeshMngr = nullptr; // Mesh Manager singleton
	MyEntityManager* m_pEntityMngr = nullptr; // Entity Manager Singleton

	vector3 m_v3Center = vector3(0.0f); // will store the center point of the octant
	vector3 m_v3Min = vector3(0.0f); // will store the minimum vector of the octant
	vector3 m_v3Max = vector3(0.0f); // will store the maximum vector of the octant

	MyOctant* m_pParent = nullptr; // will store the parent of current octant
	MyOctant* m_pChild[8]; // will store the children of the current octant

	std::vector<uint> m_EntityList; // list of entities under this octant (index in entity manager)

	MyOctant* m_pRoot = nullptr; // root octant
	std::vector<MyOctant*> m_lChild; // list of nodes that contain objects (this will be applied to root only)

public:
	MyOctant(uint a_nMaxLevel = 2, uint a_nIdealEntityCount = 5);
	MyOctant(vector3 a_v3Center, float a_fSize);
	MyOctant(MyOctant const& other);
	MyOctant& operator=(MyOctant const& other);
	~MyOctant(void);
	void Swap(MyOctant& other);
	bool IsColliding(uint a_uRBIndex);
	void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);
	void Display(vector3 a_v3Color = C_YELLOW);
	void DisplayLeafs(vector3 a_v3Color = C_YELLOW);
	void ClearEntityList(void);
	void Subdivide(void);
	bool ContainsMoreThan(uint a_nEntities);
	void KillBranches(void);
	void ConstructTree(uint a_nMaxLevel = 3);
	void AssignIDtoEntity(void);
	uint GetOctantCount(void);
	float GetSize(void);
	vector3 GetCenterGlobal(void);
	vector3 GetMinGlobal(void);
	vector3 GetMaxGlobal(void);
	MyOctant* GetChild(uint a_nChild);
	MyOctant* GetParent(void);
	bool IsLeaf(void);

private:
	void Release(void);
	void Init(void);
	void ConstructList(void);
};

}

#endif //__OCTANTCLASS_H_