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

	DisqueVirtuel::~DisqueVirtuel(){}

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
			iNode newINode = iNode(i-BASE_BLOCK_INODE, S_IFREG, 0, 0, 0);
			m_blockDisque.at(i).m_inode = &newINode;
		}
	}

	void DisqueVirtuel::initRoot(){
		//remplacer 24 par premier block libre
		m_blockDisque.at(BASE_BLOCK_INODE).m_inode->st_block = 24;
		m_blockDisque.at(BASE_BLOCK_INODE).m_inode->st_nlink = 2;
		m_blockDisque.at(BASE_BLOCK_INODE).m_inode->st_mode = S_IFDIR;
		m_blockDisque.at(24) = Block(S_IFDE);
		//peut etre a mettre dans le constructeur
		// cree . et .. dans le repertoire
		dirEntry currentDirEntry = dirEntry(BASE_BLOCK_INODE, ".");
		dirEntry parentDirEntry = dirEntry(BASE_BLOCK_INODE, "..");
		m_blockDisque.at(24).m_dirEntry.push_back(&currentDirEntry);
		m_blockDisque.at(24).m_dirEntry.push_back(&parentDirEntry);
		m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap[24] = false;
			
	}

	std::string DisqueVirtuel::bd_ls(const std::string& p_DirLocation) {
		return "G";
	}

	int DisqueVirtuel::bd_mkdir(const std::string& p_DirName) {
		//transforme le path en vecteur de nom
		//std::vector<string> path;
    		//boost::split(path, p_DirName, boost::is_any_of("/"));
		
		//m_blockDisque[positionBlockData].m_dirEntry[0]
			//>m_filename.compare("/") == 0);
		//Verifier le path
		//for (auto i: path)
			

		//Acquerir un inode libre

		//Acquerir un bloc libre

		//Creer la structure de donnee de notre directory

		//Ajouter notre directory dans le repertoire du path. Ajuster st_size 
		// l inode 

		// modifier les attributs de l<inode
	}

	int DisqueVirtuel::bd_create(const std::string& p_FileName) {
		
		return 0;
	}

	int DisqueVirtuel::bd_rm(const std::string& p_Filename) {
		return 0;
	}

	int DisqueVirtuel::findFirstEmptyINodesIndex(vector<bool> nodeVector){
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

	int DisqueVirtuel::findLastEmptyINodesIndex(vector<bool> nodeVector){
		for (int i=nodeVector.size() - 1; i>= 0; i--){
			if(nodeVector.at(i) == true){
				return i;
			}
		}
		//retourne -1 si aucun INode libre
		return -1;
	}


}//Fin du namespace
