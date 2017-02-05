// Authour: Antonio Morales.

#pragma once

#include "Runtime/XmlParser/Public/XmlParser.h"

/**
 * 
 */

struct FComboDataStruct {

	UPROPERTY()
		FString Animation;
	UPROPERTY()
		FString Particle;
	UPROPERTY()
		FString SoundFX;
	UPROPERTY()
		FString Mesh;
	UPROPERTY()
		FString Dialogue;
	//more later;

};


enum EAttackTypes {

	None,
	Light,
	Heavy,
	Light_Dash,
	Heavy_Dash

};


class RWBY_CODENAMECOLORS_API ComboNode
{
public:
	ComboNode();
	~ComboNode();

	//Node Properties
	FComboDataStruct Data;

	ComboNode* Light;
	ComboNode* Heavy;
	ComboNode* LightDash;
	ComboNode* HeavyDash;

	EAttackTypes NodeType;

	ComboNode(FString Animation, EAttackTypes Type);
	void FilleComboTree(ComboNode* Root, TArray<FString> Animation, TArray<EAttackTypes> Type);

	bool HasChildren();

	bool DoesExist(EAttackTypes Type);

	void CreateTree();

	void CreateTreeRecursively(ComboNode* CurrerntRoot, const FXmlNode* CurrentRootNode, int CurrentMainTreeBranch);

	FString GetInnnerXML(FString DesiredTag, const FXmlNode* CurrentRootNode, FString BaseToContain);

	FString GetAssetName(FString DesiredAssetType, FString AllInnerContent);


};
