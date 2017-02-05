// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "RubyRoseComboNode.h"

ComboNode::ComboNode()
{
}

ComboNode::ComboNode(FString Animation, EAttackTypes Type) {

	Data.Animation = Animation;
	NodeType = Type;

	Light = nullptr;
	Heavy = nullptr;
	LightDash = nullptr;
	HeavyDash = nullptr;
}

void ComboNode::CreateTreeRecursively(ComboNode* CurrerntRoot, const FXmlNode* CurrentRootNode, int CurrentMainTreeBranch) {

	TArray<int> MultipleChildrenPositionInBranch;

	int counter = 0;

	//base case. if at the end of a branch and there are 
	if (MultipleChildrenPositionInBranch.Num() == 0 && CurrentRootNode->GetChildrenNodes().Num() == 0 && CurrentMainTreeBranch == 4) {

		return;
	}
	//Case when the end of a branch hasn't been reached yet. travel down the most left path that hasn't been traced yet.
	else if (CurrentRootNode->GetChildrenNodes().Num() > 0) {

		//This will create the nodes for the tree for Light Attacks
		if (!CurrentRootNode->FindChildNode("Light") == NULL) {
			if (CurrentRootNode->FindChildNode("Light")->GetTag() == "Light") {
				//CurrentRootNode = CurrentRootNode->FindChildNode("Light");
				//RIGHT HERE!! YEA YOU!! make a search/get method to get the internal value of the animation node.
				CurrerntRoot->Light = new ComboNode(GetAssetName("Animation", CurrentRootNode->FindChildNode("Light")->GetContent()) , EAttackTypes::Light);
				counter++;
				if (CurrentRootNode->GetChildrenNodes().Num() > 1) {
					MultipleChildrenPositionInBranch.Add(counter);
				}

				CreateTreeRecursively(CurrerntRoot->Light, CurrentRootNode->FindChildNode("Light"), 0);
			}
		}

		//This will create the nodes for the tree for Light Attacks
		if (!CurrentRootNode->FindChildNode("Heavy") == NULL) {
			if (CurrentRootNode->FindChildNode("Heavy")->GetTag() == "Heavy") {
				CurrerntRoot->Heavy = new ComboNode(GetAssetName("Animation", CurrentRootNode->FindChildNode("Heavy")->GetContent()), EAttackTypes::Heavy);
				counter++;
				if (CurrentRootNode->GetChildrenNodes().Num() > 1) {
					MultipleChildrenPositionInBranch.Add(counter);
				}

				CreateTreeRecursively(CurrerntRoot->Heavy, CurrentRootNode->FindChildNode("Heavy"), 0);
			}
		}


		//This will create the nodes for the tree for Light Attacks
		if (!CurrentRootNode->FindChildNode("Light-Dash") == NULL) {
			if (CurrentRootNode->FindChildNode("Light-Dash")->GetTag() == "Light-Dash") {
				CurrerntRoot->LightDash = new ComboNode(GetAssetName("Animation", CurrentRootNode->FindChildNode("Light-Dash")->GetContent()), EAttackTypes::Light_Dash);
				counter++;
				if (CurrentRootNode->GetChildrenNodes().Num() > 1) {
					MultipleChildrenPositionInBranch.Add(counter);
				}

				CreateTreeRecursively(CurrerntRoot->LightDash, CurrentRootNode->FindChildNode("Light-Dash"), 0);
			}
		}


		//This will create the nodes for the tree for Light Attacks
		if (!CurrentRootNode->FindChildNode("Heavy-Dash") == NULL) {
			if (CurrentRootNode->FindChildNode("Heavy-Dash")->GetTag() == "Heavy-Dash") {
				CurrerntRoot->HeavyDash = new ComboNode(GetAssetName("Animation", CurrentRootNode->FindChildNode("Heavy-Dash")->GetContent()), EAttackTypes::Heavy_Dash);
				counter++;
				if (CurrentRootNode->GetChildrenNodes().Num() > 1) {
					MultipleChildrenPositionInBranch.Add(counter);
				}

				CreateTreeRecursively(CurrerntRoot->HeavyDash, CurrentRootNode->FindChildNode("Heavy-Dash"), 0);
			}
		}

		if (!CurrentRootNode->FindChildNode("end") == NULL) {
			if (CurrentRootNode->FindChildNode("end")->GetTag() == "end") {
				return;
			}
		}
	}
}

//Currently broken....
FString ComboNode::GetInnnerXML(FString DesiredTag, const FXmlNode* CurrentRootNode, FString BaseToContain) {

	return CurrentRootNode->FindChildNode(DesiredTag)->GetContent();
}

FString ComboNode::GetAssetName(FString DesiredAssetType, FString AllInnerContent) {
	
	FString ReturnString = "";

	TArray<FString> Types;
	AllInnerContent.ParseIntoArray(Types,TEXT(","), true);
	
	for (int i = 0; i < Types.Num(); i++) {
		Types[i].RemoveAt(0, Types[i].Find("=", ESearchCase::CaseSensitive, ESearchDir::FromStart, 0) +  1, true);
		if (DesiredAssetType == "Animation") {
			ReturnString = Types[0];
		}
		else if (DesiredAssetType == "Particle") {
			ReturnString = Types[1];
		}
		else if (DesiredAssetType == "SoundFX") {
			ReturnString = Types[2];
		}
		else if (DesiredAssetType == "Mesh") {
			ReturnString = Types[3];
		}
		else if (DesiredAssetType == "Dialogue") {
			ReturnString = Types[4];
		}
	}
	return ReturnString;
}

void ComboNode::FilleComboTree(ComboNode* Root, TArray<FString> Animation, TArray<EAttackTypes> Type) {

	for (int i = 0; i < Type.Num(); i++) {

		//Does the current root of the tree have any children nodes
		if (!Root->HasChildren()) {
			//if not then simply create the correct node for the correct combo command
			switch (Type[i]) {

			case(EAttackTypes::Light) :
				Root->Light = new ComboNode(Animation[i], Type[i]);
				Root = Root->Light;
				break;
			case(EAttackTypes::Heavy) :
				Root->Heavy = new ComboNode(Animation[i], Type[i]);
				Root = Root->Heavy;
				break;
			case(EAttackTypes::Light_Dash) :
				Root->LightDash = new ComboNode(Animation[i], Type[i]);
				Root = Root->LightDash;
				break;
			case(EAttackTypes::Heavy_Dash) :
				Root->HeavyDash = new ComboNode(Animation[i], Type[i]);
				Root = Root->HeavyDash;
				break;
			default:

				break;
			}

		}
		//we need to ignore the already created nodes, but create the correct ones with same parent
		else {

			switch (Type[i]) {

			case(EAttackTypes::Light) :

				if (Root->DoesExist(EAttackTypes::Light)) {
					Root = Root->Light;
					break;
				}
				Root->Light = new ComboNode(Animation[i], Type[i]);
				Root = Root->Light;
				break;
			case(EAttackTypes::Heavy) :
				if (Root->DoesExist(EAttackTypes::Heavy)) {
					Root = Root->Heavy;
					break;
				}
				Root->Heavy = new ComboNode(Animation[i], Type[i]);
				Root = Root->Heavy;
				break;
			case(EAttackTypes::Light_Dash) :
				if (Root->DoesExist(EAttackTypes::Light_Dash)) {
					Root = Root->LightDash;
					break;
				}
				Root->LightDash = new ComboNode(Animation[i], Type[i]);
				Root = Root->LightDash;
				break;
			case(EAttackTypes::Heavy_Dash) :
				if (Root->DoesExist(EAttackTypes::Heavy_Dash)) {
					Root = Root->HeavyDash;
					break;
				}
				Root->HeavyDash = new ComboNode(Animation[i], Type[i]);
				Root = Root->HeavyDash;
				break;
			default:

				break;
			}

		}
	}

}

ComboNode::~ComboNode()
{
}

bool ComboNode::HasChildren()
{
	if (Light != nullptr) {
		return true;
	}
	else if (Heavy != nullptr) {
		return true;
	}
	else if (LightDash != nullptr) {
		return true;
	}
	else if (HeavyDash != nullptr) {
		return true;
	}
	return false;
}

bool ComboNode::DoesExist(EAttackTypes Type) {

	switch (Type) {

	case(EAttackTypes::Light) :
		if (Light == nullptr) {
			return false;
		}
		return true;

		break;
	case(EAttackTypes::Heavy) :
		if (Heavy == nullptr) {
			return false;
		}
		return true;

		break;
	case(EAttackTypes::Light_Dash) :
		if (LightDash == nullptr) {
			return false;
		}
		return true;
		break;
	case(EAttackTypes::Heavy_Dash) :
		if (HeavyDash == nullptr) {
			return false;
		}
		return true;

		break;
	default:

		break;
	}
	return false;
}
