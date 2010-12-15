#pragma once

#include "sage3basic.h"

namespace ssa_private
{
/** Filter which determines which CFG nodes appear in the CFg used to propagate reaching definitions.
 * Getting this right is very important. If an AST node appears twice in the filtered CFG, reaching definitions will be propagated
 * to it from two distinct locations. So, each AST node must appear at most once, (with the exception of SgFunctionDefinition)*/
struct DataflowCfgFilter
{
	/** Determines if the provided CFG node should be traversed during DefUse.
	 *
	 * @param cfgn The node in question.
	 * @return Whether it should be traversed.
	 */
	bool operator() (CFGNode cfgn) const
	{
		SgNode *node = cfgn.getNode();

		switch (node->variantT())
		{
			//For function calls, we only keep the last node. The function is actually called after all its parameters
			//are evaluated.
			case V_SgFunctionCallExp:
				return (cfgn == node->cfgForEnd());

			//For basic blocks and other "container" nodes, keep the node that appears before the contents are executed
			case V_SgBasicBlock:
			case V_SgExprStatement:
			case V_SgCommaOpExp:
				return (cfgn == node->cfgForBeginning());

			//Keep the last index for initialized names. This way the def of the variable doesn't propagate to its assign
			//initializer.
			case V_SgInitializedName:
				return (cfgn == node->cfgForEnd());

			case V_SgTryStmt:
				return (cfgn == node->cfgForBeginning());

			//We only want the middle appearance of the teritatry operator - after its conditional expression
			//and before the true and false bodies. This makes it behave as an if statement for data flow
			//purposes
			case V_SgConditionalExp:
				return (cfgn.getIndex() == 1);

			default:
				break;
		}

		//Remove all non-interesting nodes
		if (!cfgn.isInteresting())
			return false;

		//Remove the midde node for logical operators with short circuiting.
		//E.g. && appears in the CFG between its LHS and RHS operands. We remove it
		//FIXME: This removes some branches in the CFG. There should be a better way to address this
		if (isSgAndOp(node) || isSgOrOp(node))
		{
			if (cfgn != node->cfgForEnd())
				return false;
		}

		return true;
	}
};
}
