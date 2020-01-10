#include <cstdlib>
#include <stdio.h>
#include <functional>   // std::greater
#include <vector>
#include <map>
#include "../Fuz-Ro-D-oh-64/FuzRoDohInternals.h"

PlayerTopicListT::ListNode<PlayerDialogData>* AggregateList(std::map<int, std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>, std::greater<int>>& candidatesMap) {
	std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*> candidates = std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>();

	bool randomStackFinished = false;
	for (std::map<int, std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>, std::greater<int>>::iterator itm = candidatesMap.begin(); itm != candidatesMap.end(); ++itm) {

		for (std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>::iterator itl = itm->second.begin(); itl != itm->second.end(); ++itl) {
			PlayerTopicListT::ListNode<PlayerDialogData>* node = *itl;
			candidates.push_back(node);

			/**
			 * If that candidate is not a random, or is marked as a random end, we'll end looping here.
			 */
			if (((node->Data->parentTopicInfo->dialogFlags & 0x2) == 0x0) || ((node->Data->parentTopicInfo->dialogFlags & 0x20) == 0x20)) {
				randomStackFinished = true;
				break;
			}

		}

		if (randomStackFinished) {
			break;
		}

	}



	std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>::iterator it = candidates.begin();
	const unsigned long n = std::distance(it, candidates.end());
	const unsigned long divisor = (RAND_MAX + 1) / n;

	unsigned long k;
	do { k = std::rand() / divisor; } while (k >= n);

	std::advance(it, k);
	return *it;
}


//! Byte swap int
uint32_t __inline swap_uint32(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | ((val >> 16) & 0xFFFF);
}

int __stdcall AggregateDialogueInfo() {

	override::MenuTopicManager* manager = (override::MenuTopicManager*)::MenuTopicManager::GetSingleton();
	if (manager->availableResponses == NULL)
		return 1;

	auto currentNode = &manager->availableResponses->Head;

	/**
	 * Map: EDID => (PRIORITY => [Entries]))
	 */
	std::map<std::string, std::map<int, std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>, std::greater<int>>> mappedNodes = std::map<std::string, std::map<int, std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>, std::greater<int>>>();

	std::map<std::string, bool> insertedNodes = std::map<std::string, bool>();

	if (currentNode->Data == NULL) {
		return 1; /* If the linked list was empty, then return */
	}

	while (currentNode->Data != NULL) {
		std::string currentTopic = std::string(currentNode->Data->title.Get());
		if (mappedNodes.find(currentTopic) == mappedNodes.end()) {
			mappedNodes[currentTopic] = std::map<int, std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>, std::greater<int>>();
		}

		uint32_t priority = swap_uint32(currentNode->Data->parentTopic->unk20);

		if (mappedNodes[currentTopic].find(priority) == mappedNodes[currentTopic].end()) {
			mappedNodes[currentTopic].insert(std::pair<int, std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>>(priority, std::vector<PlayerTopicListT::ListNode<PlayerDialogData>*>()));
		}

		mappedNodes[currentTopic][priority].push_back(currentNode);

		currentNode = currentNode->Next;

	}

	//We're now rewiring the linked list to contain the
	PlayerTopicListT::ListNode<PlayerDialogData>* previousNode = NULL;
	currentNode = &manager->availableResponses->Head;

	while (currentNode != NULL) {

		std::string currentWireTopic = std::string(currentNode->Data->title.Get());
		if (insertedNodes.find(currentWireTopic) == insertedNodes.end()) {

			/*
			 * Aggregate the list
			 */
			PlayerTopicListT::ListNode<PlayerDialogData>* chosen = AggregateList(mappedNodes[currentWireTopic]);

			/**
			 * REPLACE NODE
			 */
			currentNode->Data = chosen->Data;
			insertedNodes[currentWireTopic] = true; //Mark as inserted

			previousNode = currentNode;
		}
		else {
			/*
			 * Node already aggregated, skip this.
			 */
			if (previousNode != NULL) {
				/**
				 * REMOVE NODE
				 */
				previousNode->Next = currentNode->Next;
				//Do not move the previous node, as it will stay the same after the change.
			}
			else {
				manager->availableResponses->Head = *currentNode->Next;
			}

			/**
			 * @todo - free this node or not?
			 */
		}

		/*
		 * Move the iteration
		 */
		currentNode = currentNode->Next;


	}

	return 1; //We're overwriting the mov eax, 1 as caving target, so we have this to return eax 1

}