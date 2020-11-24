/**
 * \file disqueVirtuel.cpp
 * \brief Implémentation d'un disque virtuel.
 * \author ?
 * \version 0.1
 * \date nov-dec 2020
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

	// Ajouter votre code ici !

	DisqueVirtuel::DisqueVirtuel(){
	}
	DisqueVirtuel::~DisqueVirtuel(){
	}
	int DisqueVirtuel::bd_FormatDisk(){
	return 0;
	}
	std::string DisqueVirtuel::bd_ls(const std::string& p_DirLocation){
	
	return p_DirLocation;
	}
	int DisqueVirtuel::bd_mkdir(const std::string& p_DirName){
	return 1;
	}
	int DisqueVirtuel::bd_create(const std::string& p_FileName)	
	{
	return 1;	
	}
	int DisqueVirtuel::bd_rm(const std::string& p_Filename){
	return 0;
	}

}//Fin du namespace
