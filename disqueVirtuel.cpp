/**
 * \file disqueVirtuel.cpp
 * \brief Implémentation d'un disque virtuel.
 * \author ?
 * \version 0.1
 * \date  2021
 *
 *  Travail pratique numéro 3
 *
 */

#include "disqueVirtuel.h"
#include <iostream>
#include <string>
//vous pouvez inclure d'autres librairies si c'est nécessaire

namespace TP3
{
	DisqueVirtuel::DisqueVirtuel(){
		bd_FormatDisk();
	}

	DisqueVirtuel::~DisqueVirtuel(){
		for (int i=BASE_BLOCK_INODE; i<(BASE_BLOCK_INODE + N_INODE_ON_DISK); i++){
			delete m_blockDisque.at(i).m_inode;
		}
	}

	int DisqueVirtuel::bd_FormatDisk(){
		try{
			m_blockDisque.clear();
			// Cree les 128 blocques dans le vecteur
			for (int i=0; i<N_BLOCK_ON_DISK; i++){
				m_blockDisque.push_back(Block());
			}
			initBitmapEmptyBlocks();
			initBitmapEmptyINodes();
			initINodes();
			initRoot();
		}
		catch(...)
		{
			return 0;
		}
		return 1;
	}


	void DisqueVirtuel::initBitmapEmptyBlocks(){

		Block bitmapBlock = Block(S_IFBL);

		//Marquer les blocs 0 a 23 comme occupés, le reste comme libre.
		for (int i=0; i<(BASE_BLOCK_INODE + N_INODE_ON_DISK); i++){
			bitmapBlock.m_bitmap.push_back(false);
		}
		for (int j=BASE_BLOCK_INODE + N_INODE_ON_DISK; j<N_BLOCK_ON_DISK; j++){
			bitmapBlock.m_bitmap.push_back(true);
		}

		//Ajouter le bitmap des blocs libres sur le disque.
		m_blockDisque.at(FREE_BLOCK_BITMAP) = bitmapBlock;
	}


	void DisqueVirtuel::initBitmapEmptyINodes(){
		Block bitmapINodes = Block(S_IFIL);

		//Marquer l'i-node 1 comme occupé, le reste comme libre.
		bitmapINodes.m_bitmap.push_back(false);
		for (int i=1; i<N_INODE_ON_DISK; i++){
			bitmapINodes.m_bitmap.push_back(true);
		}
		//Ajouter le bitmap des i-nodes libres sur le disque.
		m_blockDisque.at(FREE_INODE_BITMAP) = bitmapINodes;
	}

	void DisqueVirtuel::initINodes(){
		for (int i=BASE_BLOCK_INODE; i<(BASE_BLOCK_INODE + N_INODE_ON_DISK); i++){
			m_blockDisque.at(i) = Block(S_IFIN);
			//peut etre a mettre dans le constucteur
			iNode *newINode = new iNode(i-BASE_BLOCK_INODE, S_IFREG, 0, 0, 0);
			m_blockDisque.at(i).m_inode = newINode;
		}
	}

	void DisqueVirtuel::initRoot(){
		//remplacer 24 par premier block libre
		m_blockDisque.at(BASE_BLOCK_INODE+ROOT_INODE).m_inode->st_block = BASE_BLOCK_INODE + N_INODE_ON_DISK;
		m_blockDisque.at(BASE_BLOCK_INODE+ROOT_INODE).m_inode->st_nlink = 2;
		m_blockDisque.at(BASE_BLOCK_INODE+ROOT_INODE).m_inode->st_mode = S_IFDIR;
		m_blockDisque.at(BASE_BLOCK_INODE + N_INODE_ON_DISK) = Block(S_IFDE);
		//peut etre a mettre dans le constructeur
		// cree . et .. dans le repertoire
		dirEntry currentDirEntry = dirEntry(ROOT_INODE, ".");
		dirEntry parentDirEntry = dirEntry(ROOT_INODE, "..");
		m_blockDisque.at(BASE_BLOCK_INODE + N_INODE_ON_DISK).m_dirEntry.push_back(new dirEntry(ROOT_INODE, "."));
		m_blockDisque.at(BASE_BLOCK_INODE + N_INODE_ON_DISK).m_dirEntry.push_back(new dirEntry(ROOT_INODE, ".."));
		m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap[BASE_BLOCK_INODE + N_INODE_ON_DISK] = false;
		m_blockDisque.at(FREE_INODE_BITMAP).m_bitmap[ROOT_INODE] = false;
	}
	
	void DisqueVirtuel::create_empty_repo(int inode, int block) {
		m_blockDisque.at(emptyBlock).m_type_donnees = S_IFDE;
		for (dirEntry *entry : m_blockDisque.at(emptyBlock).m_dirEntry) {
			delete entry;
		}
		m_blockDisque.at(emptyBlock).m_dirEntry.clear();
		m_blockDisque.at(emptyBlock).m_dirEntry.push_back(new dirEntry(emptyINode, "."));
		m_blockDisque.at(emptyBlock).m_dirEntry.push_back(new dirEntry(iNodeParent, ".."));
		m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap[block] = false;
		m_blockDisque.at(FREE_INODE_BITMAP).m_bitmap[inode] = false;	
	}

	std::string DisqueVirtuel::bd_ls(const std::string& p_DirLocation) {
		std::vector<std::string> a = getPathDecompose(p_DirLocation);
		/*for(std::string b:a){
			std::cout << "" << std::endl;
			std::cout << b << std::endl;
		}
			std::cout << "bef" << std::endl;
		*/
		int i = findINode(a);
		/*
		std::cout << "aft" << std::endl;
		std::cout << "rep= "  + std::to_string(i) << std::endl;
		*/
		//std::cout << std::to_string(m_blockDisque.size()) << std::endl;
		//std::cout << std::to_string(m_blockDisque.at(1).m_inode->st_size) << std::endl;
		
		
		return "not implemented yet!";
	}

	int DisqueVirtuel::bd_mkdir(const std::string& p_DirName) {
		//Faire la décomposition du string pour obtenir les noms des repertoires
		std::vector<std::string> newDirName = getPathDecompose(p_DirName);
		if (newDirName.empty()) return 0;
		std::vector<std::string> parentName = newDirName;
		parentName.pop_back();

		//Verifier si le path du parent est un path qui existe et si l'inode contient un repertoire
		int iNodeParent = findINode(parentName);
		if (iNodeParent == -1 || m_blockDisque.at(BASE_BLOCK_INODE + iNodeParent).m_inode->st_mode != S_IFDIR) return 0;
		
		//Verifier duplication
		//int iNodeFile = findINode(newDirName);
		//if (iNodeFile != -1) return 0;

		//prendre un inode et un bloc vide pour notre nouveau repertoire
		int emptyINode = findFirstEmptyINodesIndex(m_blockDisque.at(FREE_INODE_BITMAP).m_bitmap);
		int emptyBlock = findFirstEmptyINodesIndex(m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap);
		if (emptyBlock == -1 || emptyINode == -1) return 0;

		//Actualiser le bitmap
		std::cout << "UFS: Saisie i-node " << emptyINode << std::endl;
		std::cout << "UFS: Saisie bloc " << emptyBlock << std::endl;
		
		create_empty_repo(emptyINode, emptyBlock)
			
		//Modifier donnees de l'inode
		m_blockDisque.at(BASE_BLOCK_INODE + emptyINode).m_inode->st_block = emptyBlock;
		m_blockDisque.at(BASE_BLOCK_INODE + emptyINode).m_inode->st_nlink = 2;
		m_blockDisque.at(BASE_BLOCK_INODE + emptyINode).m_inode->st_mode = S_IFDIR;
		m_blockDisque.at(BASE_BLOCK_INODE + emptyINode).m_inode->st_size = 56;


		//Ajouter un dirEntry aux donnees du repertoire parent
		int blockParent = m_blockDisque.at(BASE_BLOCK_INODE + iNodeParent).m_inode->st_block;
		m_blockDisque.at(blockParent).m_dirEntry.push_back(new dirEntry(emptyINode, newDirName.back()));
		
		//Augmenter taille du repertoire parent et nb de links
		m_blockDisque.at(BASE_BLOCK_INODE + iNodeParent).m_inode->st_nlink++;
		m_blockDisque.at(BASE_BLOCK_INODE + iNodeParent).m_inode->st_size += 28;

		return 1;
	}

	int DisqueVirtuel::bd_create(const std::string& p_FileName) {
		//Faire la décomposition du string pour obtenir les noms des repertoires
		std::vector<std::string> fileName = getPathDecompose(p_FileName);
		if (fileName.empty()) return 0;
		std::vector<std::string> parentName = fileName;
		parentName.pop_back();

		//Verifier si le path du parent est un path qui existe et si l'inode contient un repertoire
		int iNodeParent = findINode(parentName);
		if (iNodeParent == -1 || m_blockDisque.at(BASE_BLOCK_INODE + iNodeParent).m_inode->st_mode != S_IFDIR) return 0;
		
		//Verifier duplication
		int iNodeFile = findINode(fileName);
		if (iNodeFile != -1) return 0;

		//prendre un inode et un bloc vide pour notre nouveau fichier
		int emptyINode = findFirstEmptyINodesIndex(m_blockDisque.at(FREE_INODE_BITMAP).m_bitmap);
		int emptyBlock = findFirstEmptyINodesIndex(m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap);
		if (emptyBlock == -1 || emptyINode == -1) return 0;

		//Actualiser le bitmap
		m_blockDisque.at(FREE_INODE_BITMAP).m_bitmap.at(emptyINode) = false;
		std::cout << "UFS: Saisie i-node " << emptyINode << std::endl;
		m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap.at(emptyBlock) = false;
		std::cout << "UFS: Saisie bloc " << emptyBlock << std::endl;

		//Modifier donnees de l'inode
		m_blockDisque.at(BASE_BLOCK_INODE + emptyINode).m_inode->st_block = emptyBlock;
		m_blockDisque.at(BASE_BLOCK_INODE + emptyINode).m_inode->st_nlink = 1;
		m_blockDisque.at(BASE_BLOCK_INODE + emptyINode).m_inode->st_mode = S_IFREG;
		m_blockDisque.at(BASE_BLOCK_INODE + emptyINode).m_inode->st_size = 0;

		//Ajouter un dirEntry aux donnees du repertoire parent
		int blockParent = m_blockDisque.at(BASE_BLOCK_INODE+iNodeParent).m_inode->st_block;
		m_blockDisque.at(blockParent).m_dirEntry.push_back(new dirEntry(emptyINode, fileName.back()));
		
		//Augmenter taille du repertoire parent
		m_blockDisque.at(BASE_BLOCK_INODE + iNodeParent).m_inode->st_size += 28;

		return 1;
	}

	int DisqueVirtuel::bd_rm(const std::string& p_Filename) {
		//verifie le path
		std::vector<std::string> pathVector = getPathDecompose(p_Filename);
		if (pathVector.empty()){
			std::cout << "Le path n<est pas valide";
			return 0;
		}
		//trouve l<inode du path et le block
		int iNodeToDelete = findINode(pathVector);
		if (iNodeToDelete == -1){
			std::cout << "Le path n<existe pas";
			return 0;
		}
		int blockToDelete = m_blockDisque.at(BASE_BLOCK_INODE+iNodeToDelete).m_inode->st_block;
		std::cout << "l<inode  est " << iNodeToDelete << " et son block est " << blockToDelete << std::endl;
		//Si le fichier est un repertoire et n<est pas vide
		if (m_blockDisque.at(BASE_BLOCK_INODE+iNodeToDelete ).m_inode->st_mode == S_IFDIR){
			if (!m_blockDisque.at(blockToDelete).m_dirEntry.empty()){
				std::cout << "le fichier est un repertoire et n<est pas vide";
				return 0;
			}
		}

		// Trouve l<inode precedent et le block
		pathVector.pop_back();
		int iNodeToUpdate = findINode(pathVector);
		int blockToUpdate = m_blockDisque.at(BASE_BLOCK_INODE+iNodeToUpdate).m_inode->st_block;
		std::cout << "l<inode precedent est " << iNodeToUpdate << " et son block est " << blockToUpdate << std::endl;

		//Decremente le st_nlink de l<inode precedent
		int iNodeToUpdate_st_nlink = m_blockDisque.at(BASE_BLOCK_INODE+iNodeToUpdate).m_inode->st_nlink;
                m_blockDisque.at(BASE_BLOCK_INODE+iNodeToUpdate).m_inode->st_nlink = iNodeToUpdate_st_nlink -1;

                //retirer le dir entry de l'inode precedent
		//std::vector<dirEntry*> dirEntry = m_blockDisque.at(blockToUpdate).m_dirEntry;
		auto it = m_blockDisque.at(blockToUpdate).m_dirEntry.begin();
		while (it != m_blockDisque.at(blockToUpdate).m_dirEntry.end()){			
			if ((*it)->m_iNode == iNodeToDelete){
				it = m_blockDisque.at(blockToUpdate).m_dirEntry.erase(it);
			}else{
				++it;
			}
		}
		// decremente le st_nlink de l<inode et efface le block
		int iNodeToDelet_st_nlink = m_blockDisque.at(BASE_BLOCK_INODE+iNodeToDelete).m_inode->st_nlink;
		m_blockDisque.at(BASE_BLOCK_INODE+iNodeToDelete).m_inode->st_nlink = iNodeToDelet_st_nlink - 1;
		m_blockDisque.at(BASE_BLOCK_INODE+iNodeToDelete).m_inode->st_nlink - 1;
		m_blockDisque.at(BASE_BLOCK_INODE+iNodeToDelete).m_inode->st_block = 0;
		m_blockDisque.at(BASE_BLOCK_INODE+iNodeToDelete).m_inode->st_size -= 28;
 
		//Libere l<inode et le block dans les bitmaps
		if(m_blockDisque.at(BASE_BLOCK_INODE+iNodeToDelete).m_inode->st_nlink == 0){
			m_blockDisque.at(FREE_INODE_BITMAP).m_bitmap.at(iNodeToDelete) = true;
			std::cout << "UFS: Relache i-node " + iNodeToDelete << std::endl;
		}
 		//m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap;
		m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap.at(blockToDelete) = true;
		std::cout << "UFS: Relache bloc " << blockToDelete << std::endl;
	}
	
	int DisqueVirtuel::findFirstEmptyINodesIndex(std::vector<bool> nodeVector) {
		int index = 0;
		for (bool node : nodeVector){
			if(node == true){
				return index;
			}
		index++;
		}
		//retourne -1 si aucun INode libre
		return -1;
	}

	int DisqueVirtuel::findLastEmptyINodesIndex(std::vector<bool> nodeVector){
		for (int i=nodeVector.size() - 1; i>= 0; i--){
			if(nodeVector.at(i) == true){
				return i;
			}
		}
		//retourne -1 si aucun INode libre
		return -1;
	}
	
	std::vector<std::string> DisqueVirtuel::getPathDecompose(const std::string& pathname){
		//Cette methode divise les parties du pathname selon les '/' et les insere dans un vecteur
		//La methode retourne un vecteur vide si le path ne commence pas par / ou s'il y a un repertoire sans nom

		std::vector<std::string> pathVector;
		int index = pathname.find_first_of('/');
		if (index != 0){
			return pathVector;
		}

		std::string temp_string = pathname;

		while (index != -1){
			temp_string = temp_string.substr(index+1);
			int temp_index = temp_string.find_first_of('/');
			if (temp_index > 0)
			{
				pathVector.push_back(temp_string.substr(0, temp_index));
			}
			else if (temp_index == -1){
				pathVector.push_back(temp_string);
			}
			else{
				pathVector.clear();
				return pathVector;
			} 
			index = temp_index;
		}

		return pathVector;
	}

	int DisqueVirtuel::findINode(const std::vector<std::string>& pathVector){
		//Cette methode le numero de l'inode correspondant au path. Si le path est invalide, donc si le repertoire n'existe pas, on retourne -1
		int iNodeNumber = ROOT_INODE;
		int nextiNodeNumber = ROOT_INODE;
		if(pathVector.size() == 1 && pathVector.at(0) == ""){return iNodeNumber + BASE_BLOCK_INODE;}
		for (int i=0; i<pathVector.size(); i++){
			std::string pathName = pathVector.at(i);
			int blockToVerify = m_blockDisque.at(nextiNodeNumber + BASE_BLOCK_INODE).m_inode->st_block;
			for (int j=0; j<m_blockDisque.at(blockToVerify).m_dirEntry.size(); j++){
				if (pathName.compare(m_blockDisque.at(blockToVerify).m_dirEntry.at(j)->m_filename) == 0){
					nextiNodeNumber = m_blockDisque.at(blockToVerify).m_dirEntry.at(j)->m_iNode;
					break;
				}
			}
			if (iNodeNumber == nextiNodeNumber) return -1;
			iNodeNumber = nextiNodeNumber;
		}
		return iNodeNumber;
	}


}//Fin du namespace
