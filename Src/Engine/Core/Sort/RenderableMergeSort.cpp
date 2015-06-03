#include "RenderableMergeSort.h"
#include "EntityCollector.h"
#include "RenderableListItemPool.h"

USING_AWAY_NAMESPACE

void RenderableMergeSort::sort(EntityCollector* collector)
{
	collector->setOpaqueRenderableHead(mergeSortByMaterial(collector->getOpaqueRenderableHead()));
	collector->setBlendedRenderableHead(mergeSortByDepth(collector->getBlendedRenderableHead()));
}

RenderableListItem* RenderableMergeSort::mergeSortByDepth(RenderableListItem* head)
{
	if (!head || !head->m_next)
		return head;

	// split in two sublists
	RenderableListItem* slow = head;
	RenderableListItem* fast = head->m_next;
	while (fast)
	{
		fast = fast->m_next;
		if (fast)
		{
			slow = slow->m_next;
			fast = fast->m_next;
		}
	}

	RenderableListItem* headB = slow->m_next;
	slow->m_next = nullptr;

	// recurse
	head = mergeSortByDepth(head);
	headB = mergeSortByDepth(headB);

	// merge sublists while respecting order
	RenderableListItem* result = nullptr;
	RenderableListItem* curr = nullptr;
	RenderableListItem* l;

	if (!head)
		return headB;

	if (!headB)
		return head;

	while (head && headB)
	{
		if (head->m_zIndex < headB->m_zIndex)
		{
			l = head;
			head = head->m_next;
		}
		else
		{
			l = headB;
			headB = headB->m_next;
		}

		if (!result)
			result = l;
		else
			curr->m_next = l;

		curr = l;
	}

	if (head)
		curr->m_next = head;
	else if (headB)
		curr->m_next = headB;

	return result;
}

RenderableListItem* RenderableMergeSort::mergeSortByMaterial(RenderableListItem* head)
{
	if (!head || !head->m_next)
		return head;

	// split in two sublists
	RenderableListItem* slow = head;
	RenderableListItem* fast = head->m_next;
	while (fast)
	{
		fast = fast->m_next;
		if (fast)
		{
			slow = slow->m_next;
			fast = fast->m_next;
		}
	}

	RenderableListItem* headB = slow->m_next;
	slow->m_next = nullptr;

	// recurse
	head = mergeSortByMaterial(head);
	headB = mergeSortByMaterial(headB);

	// merge sublists while respecting order
	RenderableListItem* result = nullptr;
	RenderableListItem* curr = nullptr;
	RenderableListItem* l;
	int cmp;

	if (!head)
		return headB;

	if (!headB)
		return head;

	while (head && headB)
	{
		// first sort per render order id (reduces program3D switches),
		// then on material id (reduces setting props),
		// then on zIndex (reduces overdraw)
		unsigned int aid = head->m_renderOrderId;
		unsigned int bid = headB->m_renderOrderId;

		if (aid == bid)
		{
			unsigned int ma = head->m_materialId;
			unsigned int mb = headB->m_materialId;

			if (ma == mb)
			{
				if (head->m_zIndex < headB->m_zIndex)
					cmp = 1;
				else
					cmp = -1;
			}
			else if (ma > mb)
				cmp = 1;
			else
				cmp = -1;
		}
		else if (aid > bid)
			cmp = 1;
		else
			cmp = -1;

		if (cmp < 0)
		{
			l = head;
			head = head->m_next;
		}
		else {
			l = headB;
			headB = headB->m_next;
		}

		if (!result)
		{
			result = l;
			curr = l;
		}
		else
		{
			curr->m_next = l;
			curr = l;
		}
	}

	if (head)
		curr->m_next = head;
	else if (headB)
		curr->m_next = headB;

	return result;
}