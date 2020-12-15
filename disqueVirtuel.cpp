
/**
 * \file disqueVirtuel.cpp
 * \brief Implémentation d'un disque virtuel.
 * \author ?
 * \version 0.1
 * \date nov-dec 2020
 *
 *  Travail pratique numero 3
 *
 */
#include "disqueVirtuel.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
namespace TP3
{
    DisqueVirtuel::DisqueVirtuel(){
        m_blockDisque.resize(N_BLOCK_ON_DISK);
    }
    DisqueVirtuel::~DisqueVirtuel(){
        for (unsigned int i = 0; i < m_blockDisque.size(); i++)
        {
            delete m_blockDisque[i].m_inode;
        }
        for (unsigned int i = 25; i < m_blockDisque.size(); i++)
        {
            for (int j=0; j < m_blockDisque[i].m_dirEntry.size(); j++){
                delete m_blockDisque[i].m_dirEntry[j];
            }
        }
        m_blockDisque.clear();
    }

    int countSlashunderscores(std::string s) {
        int count = 0;

        for (int i = 0; i < s.size(); i++)
            if (s[i] == '/') count++;

        return count;
    }


    // format a disk
    int DisqueVirtuel::bd_FormatDisk(){
        try{
            m_blockDisque.clear(); // clear the content of the disk and reinitialise.
            m_blockDisque.resize(N_BLOCK_ON_DISK);
            m_blockDisque[FREE_BLOCK_BITMAP].m_type_donnees=S_IFBL; // initialise blocks 2 et 3.
            m_blockDisque[FREE_INODE_BITMAP].m_type_donnees = S_IFIL;
            m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap.resize(N_BLOCK_ON_DISK);
            m_blockDisque[FREE_INODE_BITMAP].m_bitmap.resize(N_INODE_ON_DISK);
            for (int i =24; i < N_BLOCK_ON_DISK; i++ ){                // initialise bloc 24- 128 as free
                m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[i] = true;
            }
            for (int i = BASE_BLOCK_INODE; i <= 23 ; i++){              // create inode on blocs 4 a 23.
                m_blockDisque[i].m_type_donnees = S_IFIN;
                m_blockDisque[i].m_inode = new iNode(i - BASE_BLOCK_INODE,0, 0, 0, 0);
            }
            for (int i=1; i < N_INODE_ON_DISK; i++){    //  free all nodes except  No 0
                m_blockDisque[FREE_INODE_BITMAP].m_bitmap[i] = true;
            }
            int freeBlock = findFreeBlock();
            int freeInode = findFreeInode();
            createEmptyRepo(freeInode, freeInode, freeBlock);
        }
        catch (...) {
            return 0;
        }
        return 1;
    }
    // create a file
    int DisqueVirtuel::bd_create(const std::string& p_FileName){
        std::string path, file;
        getLastRepo(p_FileName, path, file);
        if(findRepertoryExist(path)){
            if (fileExist(p_FileName) == false){
                int iNodeLibre = findFreeInode();  //find block and inode parent
                int iNodeParent = findINodeWithAPath(path);
                int blocParent = m_blockDisque.at(iNodeParent + BASE_BLOCK_INODE).m_inode->st_block;
                dirEntry *pFichier = new dirEntry(iNodeLibre, file ); //create a file
                m_blockDisque.at(blocParent).m_dirEntry.push_back(pFichier);    //add a file in parent directory
                m_blockDisque.at(iNodeLibre + BASE_BLOCK_INODE).m_inode->st_mode = S_IFREG;
                m_blockDisque.at(iNodeLibre + BASE_BLOCK_INODE).m_inode->st_nlink = updateLink(iNodeLibre);
                m_blockDisque.at(iNodeLibre + BASE_BLOCK_INODE).m_inode->st_size = 0; // put the size of the file to 0.
                updateBitmapInode(iNodeLibre, false); // update  bitmap
                updateParentInode(iNodeParent); //update inode parent
                return 1;
            }
            std::cout << "The file exist aready "<<std::endl;
            return 0;
        }
        std::cout << "Repository doesn't exist " << std::endl;
        return 0;
    }
    // create a repertory
    int DisqueVirtuel::bd_mkdir(const std::string& p_DirName){
        std::string path,Dir;
        getLastRepo(p_DirName, path, Dir);
        if (findRepertoryExist(path)){  //check if the repo exist.
            if (fileExist(p_DirName) == false){ // verify if the file exist.
                int blocLibre = findFreeBlock();
                int iNodeLibre = findFreeInode();
                int iNodeParent = findINodeWithAPath(path);    //find free block and freeInode.
                int blocParent = m_blockDisque.at(iNodeParent + BASE_BLOCK_INODE).m_inode->st_block;
                dirEntry *pRep = new dirEntry(iNodeLibre, Dir ); //create repo
                m_blockDisque.at(blocParent).m_dirEntry.push_back(pRep);
                createEmptyRepo(iNodeParent, iNodeLibre, blocLibre);
                updateParentInode(iNodeParent);
                return 1;
            }
            std::cout << "Repository exist arleady "<< std::endl;
            return 0;
        }
        std::cout << "Repository doesn't exist arleady"<< std::endl;
        return 0;
    }
    // display files and repository which is in parent repo.
    std::string DisqueVirtuel::bd_ls(const std::string& p_DirLocation){
        std::string path, findPath;
        getLastRepo(p_DirLocation, path, findPath);
        int inode;
        if (path == "/" && findPath == "") inode = ROOT_INODE;
        else inode = findINodeWithAPath(findPath);
        int block = m_blockDisque.at(inode + BASE_BLOCK_INODE).m_inode->st_block;
        int space = 12;
        std::ostringstream os;
        if (findRepertoryExist(p_DirLocation)) {
            os << p_DirLocation << std::endl;
            for (int i = 0; i < m_blockDisque.at(block).m_dirEntry.size(); i++) {
                int j = m_blockDisque.at(block).m_dirEntry.at(i)->m_iNode;
                if (m_blockDisque.at(j + BASE_BLOCK_INODE).m_inode->st_mode == S_IFDIR) {
                    os << "d" << std::setw(space + 1) << m_blockDisque.at(block).m_dirEntry.at(i)->m_filename
                       << "  Size:" << std::setw(space / 2) << m_blockDisque.at(j + BASE_BLOCK_INODE).m_inode->st_size
                       << "  inode:" << std::setw(space / 2) << m_blockDisque.at(j + BASE_BLOCK_INODE).m_inode->st_ino
                       << "  nlink:" << std::setw(space / 2) << m_blockDisque.at(j + BASE_BLOCK_INODE).m_inode->st_nlink
                       << std::endl;
                } else {
                    os << "-" << std::setw(space + 1) << m_blockDisque.at(block).m_dirEntry.at(i)->m_filename
                       << "  Size:" << std::setw(space / 2) << m_blockDisque.at(j + BASE_BLOCK_INODE).m_inode->st_size
                       << "  inode:" << std::setw(space / 2) << m_blockDisque.at(j + BASE_BLOCK_INODE).m_inode->st_ino
                       << "  nlink:" << std::setw(space / 2) << m_blockDisque.at(j + BASE_BLOCK_INODE).m_inode->st_nlink
                       << std::endl;
                }
            }
        }
        else os << "Directory doesn't exist "  << std::endl ;
        return os.str();
    }
    //remove a file or a repository
    int DisqueVirtuel::bd_rm(const std::string& p_Filename) {
        if (!fileExist(p_Filename)){
            std::cout << "File not found in repository!"  << std::endl;
            return 0;
        }
        std::string path, del;
        getLastRepo(p_Filename, path, del);
        int iNodeParentDeleted = findINodeWithAPath(path); //find prent inode.
        int blocParentDeleted = m_blockDisque.at(iNodeParentDeleted + BASE_BLOCK_INODE).m_inode->st_block;
        int deletedINode = findINodeWithAPath(p_Filename); //find inode of the file to delete
        int deletedBlock = m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_block;
        if (m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_mode == S_IFDIR) {
            if (m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_size == 2 * 28) {
                m_blockDisque.at(deletedBlock).m_dirEntry.clear(); //delete parent
                m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_nlink = updateLink(deletedINode);
                for (int i = 0; i < m_blockDisque.at(
                        blocParentDeleted).m_dirEntry.size(); i++) {
                    if (m_blockDisque.at(blocParentDeleted).m_dirEntry.at(i)->m_filename == del) {
                        m_blockDisque.at(blocParentDeleted).m_dirEntry.erase(m_blockDisque.at(blocParentDeleted).m_dirEntry.begin() + i);
                        updateParentInode(iNodeParentDeleted);
                        // free inode
                        m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_nlink = 0;
                        m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_mode = 0;
                        m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_block = 0;
                        m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_size = 0;
                        updateBitmapInode(deletedINode, true);
                        updateBitmapBlock(deletedBlock, true);
                        return 1;
                    }
                }
            } else {
                std::cout << "You can't delete this file"<< std::endl;
                return 0;
            }
        }
        // file
        else {
            m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_nlink = m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_nlink - 1;
            if (m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_nlink == 0) {
                for (int i = 0; i < m_blockDisque.at(
                        blocParentDeleted).m_dirEntry.size(); i++) { // find the parent
                    if (m_blockDisque.at(blocParentDeleted).m_dirEntry.at(i)->m_filename == del) {
                        m_blockDisque.at(blocParentDeleted).m_dirEntry.erase(m_blockDisque.at(blocParentDeleted).m_dirEntry.begin() + i);
                        //update parent.
                        updateParentInode(iNodeParentDeleted);
                        m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_nlink = 0;
                        m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_mode = 0;
                        m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_block = 0;
                        m_blockDisque.at(deletedINode + BASE_BLOCK_INODE).m_inode->st_size = 0;
                        updateBitmapInode(deletedINode, true);
                        return 1;
                    }
                }
            } else {
                std::cout << "You can't delete the file it is referenced "<< std::endl;
                return 0;
            }
        }
    }

    // create an empty repository with a parent repositorry and a current repository
    void DisqueVirtuel::createEmptyRepo(int parentInode, int freeInode, int freeBlock ){
        m_blockDisque[freeBlock].m_type_donnees = S_IFDE;
        dirEntry *p1 = new dirEntry(freeInode, ".");
        m_blockDisque[freeBlock].m_dirEntry.push_back(p1);
        dirEntry *p2 = new dirEntry(parentInode, "..");
        m_blockDisque[freeBlock].m_dirEntry.push_back(p2);
        m_blockDisque[freeInode + BASE_BLOCK_INODE].m_inode->st_mode = S_IFDIR;
        m_blockDisque[freeInode + BASE_BLOCK_INODE].m_inode->st_block = freeBlock;
        m_blockDisque[freeInode + BASE_BLOCK_INODE].m_inode->st_nlink = updateLink(freeInode);
        m_blockDisque[freeInode + BASE_BLOCK_INODE].m_inode->st_size = m_blockDisque[freeBlock].m_dirEntry.size() * 28;
        updateBitmapInode(freeInode, false);
        updateBitmapBlock(freeBlock, false);
    }

    // calculate how many times a repository has been referenced
    int DisqueVirtuel::updateLink(int nbInode){
        int links = 0;
        if (m_blockDisque.at(nbInode + BASE_BLOCK_INODE).m_inode->st_mode == S_IFREG)
            links = m_blockDisque.at(nbInode + BASE_BLOCK_INODE).m_inode->st_nlink + 1;

        else {
            int bloc = m_blockDisque.at(nbInode + BASE_BLOCK_INODE).m_inode->st_block;
            for (int i=0; i < m_blockDisque.at(bloc).m_dirEntry.size(); i++ ){
                int inodeReference = m_blockDisque.at(bloc).m_dirEntry.at(i)->m_iNode;
                if (m_blockDisque.at(inodeReference + BASE_BLOCK_INODE).m_inode->st_mode != S_IFREG ) links++;
            }
        }
        return links;
    }
    void DisqueVirtuel::updateBitmapInode(int inode, bool valide){   //update bitmap Inode
        m_blockDisque.at(FREE_INODE_BITMAP).m_bitmap.at(inode) = valide;
        displayMessageInode(valide, inode );
    }
    void DisqueVirtuel::updateBitmapBlock(int bloc, bool valide){   //update bitmap block
        m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap.at(bloc)= valide;
        displayMessageBlock(valide, bloc);
    }
    void DisqueVirtuel::displayMessageBlock(bool valide, int bloc){   // diplay message when release or use a block
        if (valide == true) std:: cout << "UFS: Relache bloc " << bloc << std::endl;
        else std:: cout << "UFS: saisir bloc " << bloc << std::endl;

    }
    void DisqueVirtuel::displayMessageInode(bool valide, int inode){   // display message when release or use inode.
        if (valide == true) std:: cout << "UFS: Relache i-node " << inode << std::endl;
        else std:: cout << "UFS: saisir i-node " << inode << std::endl;

    }
    int DisqueVirtuel::findFreeBlock(){    // find a free block
        for(int i= 0; i < N_BLOCK_ON_DISK; i++){
            if (m_blockDisque.at(FREE_BLOCK_BITMAP).m_bitmap.at(i) == true){
                return i;
            }
        }
    }
    int DisqueVirtuel::findFreeInode(){     // find a free inode

        for(int i= 0; i < N_INODE_ON_DISK; i++){
            if (m_blockDisque.at(FREE_INODE_BITMAP).m_bitmap.at(i) == true) return i;
        }
    }
    // get last repo
    void DisqueVirtuel::getLastRepo(const std::string pathRepo, std::string& path, std::string& fileName){
        int el = pathRepo.find_last_of("/");
        if (el == 0){
            path = pathRepo.front();
            fileName = pathRepo.substr(el + 1);
        }
        else{
            path = pathRepo.substr(0, el);
            fileName = pathRepo.substr(el + 1);
        }
    }

    int DisqueVirtuel::countSlashunderscores(const std::string path) {
        int count = 0;

        for (int i = 0; i < path.size(); i++)
            if (path[i] == '/') count++;

        return count;
    }

    // verify if the repertory exist
    bool DisqueVirtuel::findRepertoryExist(const std::string path){
        std::string newPath = path;
        if (newPath.size() == 1 && newPath.front() == '/') return true;
        if (newPath.front() == '/') newPath.erase(0, 1);
        int control = countSlashunderscores(newPath);
        std::size_t found = newPath.find_first_of("/"); // get parent repository
        std::string findDir = newPath.substr(0, found);
        std::string remain = newPath.substr(found + 1);
        for(int i = BASE_BLOCK_INODE; i < N_INODE_ON_DISK; i++){
            if (m_blockDisque.at(i).m_inode->st_mode == S_IFDIR){
                int blocDonnes = m_blockDisque.at(i).m_inode->st_block;
                for (int j = 0; j < m_blockDisque.at(blocDonnes).m_dirEntry.size(); j++){
                    if (m_blockDisque.at(blocDonnes).m_dirEntry.at(j)->m_filename.compare(findDir) == 0){
                        control--;
                        if (control >= 0)return findRepertoryExist(remain);
                        else return true;
                    }
                }
            }
        }
        return false;
    }
    //verify if the file exist
    bool DisqueVirtuel::fileExist(const std::string name){
        std::string copiePath = name;
        int compt = countSlashunderscores(copiePath);
        if ( compt != 1 && copiePath.front() == '/') copiePath.erase(0,1);
        std::string dirParent, nomCherche;
        getLastRepo(copiePath, dirParent, nomCherche);
        int iNodeDirParent = findINodeWithAPath(dirParent);
        int blocDireParent = m_blockDisque.at(iNodeDirParent + BASE_BLOCK_INODE ).m_inode->st_block;
        for (int j = 0; j < m_blockDisque.at(blocDireParent).m_dirEntry.size(); j++){
            if (m_blockDisque.at(blocDireParent).m_dirEntry.at(j)->m_filename.compare(nomCherche) == 0)return true;
        }
        return false;
    }
    // find number of inode using a path
    int DisqueVirtuel::findINodeWithAPath(const std::string path){
        if (path == "/") return ROOT_INODE;
        std::string inode, remain;
        getLastRepo(path, remain, inode);
        for(int i = BASE_BLOCK_INODE; i < N_INODE_ON_DISK; i++){
            int blocDonnes = m_blockDisque.at(i).m_inode->st_block;
            for (int j = 0; j < m_blockDisque.at(blocDonnes).m_dirEntry.size(); j++){
                if (m_blockDisque.at(blocDonnes).m_dirEntry.at(j)->m_filename.compare(inode) == 0){
                    return  m_blockDisque.at(blocDonnes).m_dirEntry.at(j)->m_iNode;
                }
            }
        }
    }
    // update parent inode
    void DisqueVirtuel::updateParentInode(size_t nbInode){
        int bloc = m_blockDisque.at(nbInode + BASE_BLOCK_INODE).m_inode->st_block;
        m_blockDisque.at(nbInode + BASE_BLOCK_INODE).m_inode->st_nlink = updateLink(nbInode);
        m_blockDisque.at(nbInode + BASE_BLOCK_INODE).m_inode->st_size = m_blockDisque.at(bloc).m_dirEntry.size() * 28;
    }

}//Fin du namespace