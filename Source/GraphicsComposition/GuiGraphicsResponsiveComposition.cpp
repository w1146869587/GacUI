#include "GuiGraphicsResponsiveComposition.h"
#include "../Controls/GuiBasicControls.h"

namespace vl
{
	namespace presentation
	{
		namespace compositions
		{
			using namespace collections;

/***********************************************************************
GuiResponsiveCompositionBase
***********************************************************************/

			void GuiResponsiveCompositionBase::OnParentLineChanged()
			{
				GuiBoundsComposition::OnParentLineChanged();
				GuiResponsiveCompositionBase* responsive = nullptr;
				{
					auto parent = GetParent();
					while (parent)
					{
						if (responsive = dynamic_cast<GuiResponsiveCompositionBase*>(parent))
						{
							break;
						}
						parent = parent->GetParent();
					}
				}

				if (responsiveParent != responsive)
				{
					if (responsiveParent)
					{
						responsiveParent->OnResponsiveChildRemoved(this);
						responsiveParent->OnResponsiveChildLevelUpdated();
					}
					responsiveParent = responsive;
					if (responsiveParent)
					{
						responsiveParent->OnResponsiveChildInserted(this);
						responsiveParent->OnResponsiveChildLevelUpdated();
					}
				}
			}

			void GuiResponsiveCompositionBase::OnResponsiveChildInserted(GuiResponsiveCompositionBase* child)
			{
			}

			void GuiResponsiveCompositionBase::OnResponsiveChildRemoved(GuiResponsiveCompositionBase* child)
			{
			}

			void GuiResponsiveCompositionBase::OnResponsiveChildLevelUpdated()
			{
				if (responsiveParent)
				{
					responsiveParent->OnResponsiveChildLevelUpdated();
				}
			}

			GuiResponsiveCompositionBase::GuiResponsiveCompositionBase()
			{
				SetMinSizeLimitation(LimitToElementAndChildren);
			}

			GuiResponsiveCompositionBase::~GuiResponsiveCompositionBase()
			{
			}

			ResponsiveDirection GuiResponsiveCompositionBase::GetDirection()
			{
				return direction;
			}

			void GuiResponsiveCompositionBase::SetDirection(ResponsiveDirection value)
			{
				if (direction != value)
				{
					direction = value;
					OnResponsiveChildLevelUpdated();
				}
			}

/***********************************************************************
GuiResponsiveSharedCollection
***********************************************************************/

			bool GuiResponsiveSharedCollection::QueryInsert(vint index, controls::GuiControl* const& value)
			{
				return !Contains(value) && !value->GetBoundsComposition()->GetParent();
			}

			void GuiResponsiveSharedCollection::BeforeInsert(vint index, controls::GuiControl* const& value)
			{
				view->skipUpdatingLevels = true;
			}

			void GuiResponsiveSharedCollection::AfterInsert(vint index, controls::GuiControl* const& value)
			{
				view->skipUpdatingLevels = false;
				view->OnResponsiveChildLevelUpdated();
			}

			void GuiResponsiveSharedCollection::BeforeRemove(vint index, controls::GuiControl* const& value)
			{
				CHECK_ERROR(!value->GetBoundsComposition()->GetParent(), L"GuiResponsiveSharedCollection::BeforeRemove(vint, GuiResponsiveSharedCollection* const&)#Cannot remove a shared control that is current in use.");
				view->skipUpdatingLevels = true;
			}

			void GuiResponsiveSharedCollection::AfterRemove(vint index, vint count)
			{
				view->skipUpdatingLevels = false;
				view->OnResponsiveChildLevelUpdated();
			}

			GuiResponsiveSharedCollection::GuiResponsiveSharedCollection(GuiResponsiveViewComposition* _view)
				:view(_view)
			{
			}

			GuiResponsiveSharedCollection::~GuiResponsiveSharedCollection()
			{
			}

/***********************************************************************
GuiResponsiveViewCollection
***********************************************************************/

			bool GuiResponsiveViewCollection::QueryInsert(vint index, GuiResponsiveCompositionBase* const& value)
			{
				return !Contains(value) && value->GetParent();
			}

			void GuiResponsiveViewCollection::BeforeInsert(vint index, GuiResponsiveCompositionBase* const& value)
			{
				view->skipUpdatingLevels = true;
			}

			void GuiResponsiveViewCollection::AfterInsert(vint index, GuiResponsiveCompositionBase* const& value)
			{
				if (!view->currentView)
				{
					view->currentView = value;
					view->currentView->SetAlignmentToParent(Margin(0, 0, 0, 0));
					view->AddChild(view->currentView);
				}
				view->skipUpdatingLevels = false;
				view->OnResponsiveChildLevelUpdated();
			}

			void GuiResponsiveViewCollection::BeforeRemove(vint index, GuiResponsiveCompositionBase* const& value)
			{
				CHECK_ERROR(!value->GetParent(), L"GuiResponsiveViewCollection::BeforeRemove(vint, GuiResponsiveCompositionBase* const&)#Cannot remove a view that is current in use.");
				view->skipUpdatingLevels = true;
			}

			void GuiResponsiveViewCollection::AfterRemove(vint index, vint count)
			{
				view->skipUpdatingLevels = false;
				view->OnResponsiveChildLevelUpdated();
			}

			GuiResponsiveViewCollection::GuiResponsiveViewCollection(GuiResponsiveViewComposition* _view)
				:view(_view)
			{
			}

			GuiResponsiveViewCollection::~GuiResponsiveViewCollection()
			{
			}

/***********************************************************************
GuiResponsiveSharedComposition
***********************************************************************/

			void GuiResponsiveSharedComposition::OnParentLineChanged()
			{
				GuiBoundsComposition::OnParentLineChanged();
				GuiResponsiveViewComposition* view = nullptr;
				{
					auto parent = GetParent();
					while (parent)
					{
						if (view = dynamic_cast<GuiResponsiveViewComposition*>(parent))
						{
							break;
						}
						parent = parent->GetParent();
					}
				}

				auto sharedParent = shared->GetBoundsComposition()->GetParent();
				if (sharedParent == this)
				{
					if (!view)
					{
						RemoveChild(shared->GetBoundsComposition());
					}
				}
				else if (sharedParent == nullptr)
				{
					if (view)
					{
						shared->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
						AddChild(shared->GetBoundsComposition());
					}
				}
				else
				{
					CHECK_FAIL(L"GuiResponsiveSharedComposition::OnParentLineChanged()#The specified shared control has not been released. This usually means this control is not in GuiResponsiveViewComposition::GetSharedControls().");
				}
			}

			GuiResponsiveSharedComposition::GuiResponsiveSharedComposition(controls::GuiControl* _shared)
				:shared(_shared)
			{
				SetMinSizeLimitation(LimitToElementAndChildren);
			}

			GuiResponsiveSharedComposition::~GuiResponsiveSharedComposition()
			{
			}

/***********************************************************************
GuiResponsiveViewComposition
***********************************************************************/

			void GuiResponsiveViewComposition::CalculateLevelCount()
			{
				if (views.Count() == 0)
				{
					levelCount = 1;
				}
				else
				{
					levelCount = 0;
					for (vint i = 0; i < views.Count(); i++)
					{
						auto view = views[i];
						if (((vint)direction & (vint)view->GetDirection()) != 0)
						{
							levelCount += view->GetLevelCount();
						}
						else
						{
							levelCount += 1;
						}
					}
				}
			}

			void GuiResponsiveViewComposition::CalculateCurrentLevel()
			{
				currentLevel = 0;
				for (vint i = views.Count() - 1; i >= 0; i--)
				{
					auto view = views[i];
					if (((vint)direction & (vint)view->GetDirection()) != 0)
					{
						if (currentView == view)
						{
							levelCount += view->GetCurrentLevel() + 1;
						}
						else
						{
							levelCount += view->GetLevelCount();
						}
					}
					else
					{
						levelCount++;
					}
				}
				currentLevel--;
			}

			void GuiResponsiveViewComposition::OnResponsiveChildLevelUpdated()
			{
				if (!skipUpdatingLevels)
				{
					CalculateLevelCount();
					CalculateCurrentLevel();
					GuiResponsiveCompositionBase::OnResponsiveChildLevelUpdated();
				}
			}

			GuiResponsiveViewComposition::GuiResponsiveViewComposition()
				:sharedControls(this)
				, views(this)
			{
			}

			GuiResponsiveViewComposition::~GuiResponsiveViewComposition()
			{
				if (currentView)
				{
					RemoveChild(currentView);
				}

				for (vint i = 0; i < views.Count(); i++)
				{
					SafeDeleteComposition(views[i]);
				}
				for (vint i = 0; i < sharedControls.Count(); i++)
				{
					SafeDeleteControl(sharedControls[i]);
				}
			}

			vint GuiResponsiveViewComposition::GetLevelCount()
			{
				return levelCount;
			}

			vint GuiResponsiveViewComposition::GetCurrentLevel()
			{
				return currentLevel;
			}

			bool GuiResponsiveViewComposition::LevelDown()
			{
				vint level = currentLevel;
				skipUpdatingLevels = true;
				if (!currentView->LevelDown())
				{
					vint index = views.IndexOf(currentView);
					if (index > 0)
					{
						RemoveChild(currentView);
						currentView = views[index - 1];
						currentView->SetAlignmentToParent(Margin(0, 0, 0, 0));
						CalculateCurrentLevel();
					}
				}
				skipUpdatingLevels = false;
				return level != currentLevel;
			}

			bool GuiResponsiveViewComposition::LevelUp()
			{
				vint level = currentLevel;
				skipUpdatingLevels = true;
				if (!currentView->LevelUp())
				{
					vint index = views.IndexOf(currentView);
					if (index < views.Count() + 1)
					{
						RemoveChild(currentView);
						currentView = views[index + 1];
						currentView->SetAlignmentToParent(Margin(0, 0, 0, 0));
						AddChild(currentView);
						CalculateCurrentLevel();
					}
				}
				skipUpdatingLevels = false;
				return level != currentLevel;
			}

			collections::ObservableListBase<controls::GuiControl*>& GuiResponsiveViewComposition::GetSharedControls()
			{
				return sharedControls;
			}

			collections::ObservableListBase<GuiResponsiveCompositionBase*>& GuiResponsiveViewComposition::GetViews()
			{
				return views;
			}

/***********************************************************************
GuiResponsiveFixedComposition
***********************************************************************/

			void GuiResponsiveFixedComposition::OnResponsiveChildLevelUpdated()
			{
			}

			GuiResponsiveFixedComposition::GuiResponsiveFixedComposition()
			{
			}

			GuiResponsiveFixedComposition::~GuiResponsiveFixedComposition()
			{
			}

			vint GuiResponsiveFixedComposition::GetLevelCount()
			{
				return 1;
			}

			vint GuiResponsiveFixedComposition::GetCurrentLevel()
			{
				return 0;
			}

			bool GuiResponsiveFixedComposition::LevelDown()
			{
				return false;
			}

			bool GuiResponsiveFixedComposition::LevelUp()
			{
				return false;
			}

/***********************************************************************
GuiResponsiveStackComposition
***********************************************************************/

#define DEFINE_AVAILABLE \
			auto availables = From(responsiveChildren) \
				.Where([=](GuiResponsiveCompositionBase* child) \
				{ \
					return ((vint)direction & (vint)child->GetDirection()) != 0; \
				}) \

			void GuiResponsiveStackComposition::CalculateLevelCount()
			{
				DEFINE_AVAILABLE;
				if (availables.IsEmpty())
				{
					levelCount = 1;
				}
				else
				{
					levelCount = availables
						.Select([](GuiResponsiveCompositionBase* child)
							{
								return child->GetLevelCount();
							})
						.Aggregate([](vint a, vint b)
							{
								return a + b;
							});
				}
			}

			void GuiResponsiveStackComposition::CalculateCurrentLevel()
			{
				DEFINE_AVAILABLE;
				if (availables.IsEmpty())
				{
					currentLevel = 0;
				}
				else
				{
					currentLevel = availables
						.Select([](GuiResponsiveCompositionBase* child)
							{
								return child->GetCurrentLevel() + 1;
							})
						.Aggregate([](vint a, vint b)
							{
								return a + b;
							}) - 1;
				}
			}

			void GuiResponsiveStackComposition::OnResponsiveChildInserted(GuiResponsiveCompositionBase* child)
			{
				responsiveChildren.Add(child);
			}

			void GuiResponsiveStackComposition::OnResponsiveChildRemoved(GuiResponsiveCompositionBase* child)
			{
				responsiveChildren.Remove(child);
			}

			void GuiResponsiveStackComposition::OnResponsiveChildLevelUpdated()
			{
				CalculateLevelCount();
				CalculateCurrentLevel();
				GuiResponsiveCompositionBase::OnResponsiveChildLevelUpdated();
			}

			bool GuiResponsiveStackComposition::ChangeLevel(bool levelDown)
			{
				DEFINE_AVAILABLE;

				vint level = currentLevel;
				SortedList<GuiResponsiveCompositionBase*> ignored;
				while (true)
				{
					GuiResponsiveCompositionBase* selected = false;
					vint size = 0;

					FOREACH(GuiResponsiveCompositionBase*, child, availables)
					{
						if (!ignored.Contains(child))
						{
							Size childSize = child->GetPreferredBounds().GetSize();
							vint childSizeToCompare =
								direction == ResponsiveDirection::Horizontal ? childSize.x :
								direction == ResponsiveDirection::Vertical ? childSize.y :
								childSize.x * childSize.y;

							if (!selected || (levelDown ? size < childSizeToCompare : size > childSizeToCompare))
							{
								selected = child;
								size = childSizeToCompare;
							}
						}
					}

					if (selected)
					{
						if (!(levelDown ? selected->LevelDown() : selected->LevelUp()))
						{
							ignored.Add(selected);
						}
					}
					else
					{
						break;
					}
				}

				CalculateCurrentLevel();
				return level != currentLevel;
			}

			GuiResponsiveStackComposition::GuiResponsiveStackComposition()
			{
			}

			GuiResponsiveStackComposition::~GuiResponsiveStackComposition()
			{
			}

			vint GuiResponsiveStackComposition::GetLevelCount()
			{
				return levelCount;
			}

			vint GuiResponsiveStackComposition::GetCurrentLevel()
			{
				return currentLevel;
			}

			bool GuiResponsiveStackComposition::LevelDown()
			{
				return ChangeLevel(true);
			}

			bool GuiResponsiveStackComposition::LevelUp()
			{
				return ChangeLevel(false);
			}

/***********************************************************************
GuiResponsiveGroupComposition
***********************************************************************/

			void GuiResponsiveGroupComposition::CalculateLevelCount()
			{
				DEFINE_AVAILABLE;
				if (availables.IsEmpty())
				{
					levelCount = 1;
				}
				else
				{
					levelCount = availables
						.Select([](GuiResponsiveCompositionBase* child)
							{
								return child->GetLevelCount();
							})
						.Max();
				}
			}

			void GuiResponsiveGroupComposition::CalculateCurrentLevel()
			{
				DEFINE_AVAILABLE;
				if (availables.IsEmpty())
				{
					currentLevel = 0;
				}
				else
				{
					currentLevel = availables
						.Select([](GuiResponsiveCompositionBase* child)
							{
								return child->GetCurrentLevel();
							})
						.Max();
				}
			}

			void GuiResponsiveGroupComposition::OnResponsiveChildInserted(GuiResponsiveCompositionBase* child)
			{
				responsiveChildren.Add(child);
			}

			void GuiResponsiveGroupComposition::OnResponsiveChildRemoved(GuiResponsiveCompositionBase* child)
			{
				responsiveChildren.Remove(child);
			}

			void GuiResponsiveGroupComposition::OnResponsiveChildLevelUpdated()
			{
				CalculateLevelCount();
				CalculateCurrentLevel();
				GuiResponsiveCompositionBase::OnResponsiveChildLevelUpdated();
			}

			GuiResponsiveGroupComposition::GuiResponsiveGroupComposition()
			{
			}

			GuiResponsiveGroupComposition::~GuiResponsiveGroupComposition()
			{
			}

			vint GuiResponsiveGroupComposition::GetLevelCount()
			{
				return levelCount;
			}

			vint GuiResponsiveGroupComposition::GetCurrentLevel()
			{
				return currentLevel;
			}

			bool GuiResponsiveGroupComposition::LevelDown()
			{
				DEFINE_AVAILABLE;
				vint level = currentLevel;
				FOREACH(GuiResponsiveCompositionBase*, child, availables)
				{
					if (child->GetCurrentLevel() >= level)
					{
						child->LevelDown();
					}
				}

				CalculateCurrentLevel();
				return level != currentLevel;
			}

			bool GuiResponsiveGroupComposition::LevelUp()
			{
				DEFINE_AVAILABLE;
				vint level = currentLevel;
				FOREACH(GuiResponsiveCompositionBase*, child, availables)
				{
					while (child->GetCurrentLevel() <= level)
					{
						child->LevelUp();
					}
				}

				CalculateCurrentLevel();
				return level != currentLevel;
			}

#undef DEFINE_AVAILABLE
		}
	}
}
