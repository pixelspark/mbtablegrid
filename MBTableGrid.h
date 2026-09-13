/*
 Copyright (c) 2008 Matthew Ball - http://www.mattballdesign.com
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 */

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@class MBTableGridHeaderView, MBTableGridFooterView, MBTableGridContentView;
@class MBTableGridCell, MBTableGridHeaderCell;
@protocol MBTableGridDelegate, MBTableGridDataSource;

/* Notifications */

/**
 * @brief		Posted after an MBTableGrid object's selection changes.
 *				The notification object is the table grid whose selection
 *				changed. This notification does not contain a userInfo
 *				dictionary.
 *
 * @details		This notification will often be posted twice for a single
 *				selection change (once for the column selection and once 
 *				for the row selection). As such, any methods called in
 *				response to this notification should be especially efficient.
 */
APPKIT_EXTERN NSString *MBTableGridDidChangeSelectionNotification;

/**
 * @brief		Posted after one or more columns are moved by user action
 *				in an MBTableGrid object. The notification object is
 *				the table grid in which the column(s) moved. The \c userInfo
 *				dictionary contains the following information:
 *				- \c @"OldColumns": An NSIndexSet containing the columns'
 *					original indices.
 *				- \c @"NewColumns": An NSIndexSet containing the columns'
 *					new indices.
 */
APPKIT_EXTERN NSString *MBTableGridDidMoveColumnsNotification;

/**
 * @brief		Posted after one or more rows are moved by user action
 *				in an MBTableGrid object. The notification object is
 *				the table grid in which the row(s) moved. The userInfo
 *				dictionary contains the following information:
 *				- \c @"OldRows": An NSIndexSet containing the rows'
 *					original indices.
 *				- \c @"NewRows": An NSIndexSet containing the rows'
 *					new indices.
 */
APPKIT_EXTERN NSString *MBTableGridDidMoveRowsNotification;

APPKIT_EXTERN NSString *MBTableGridColumnDataType;
APPKIT_EXTERN NSString *MBTableGridRowDataType;

typedef NS_ENUM(NSUInteger, MBHorizontalEdge) {
    MBHorizontalEdgeLeft,
    MBHorizontalEdgeRight
};

typedef NS_ENUM(NSUInteger, MBVerticalEdge) {
    MBVerticalEdgeTop,
    MBVerticalEdgeBottom
};

/**
 * @brief		MBTableGrid (sometimes referred to as a table grid)
 *				is a means of displaying tabular data in a spreadsheet
 *				format.
 *
 * @details		An MBTableGrid object must have an object that acts
 *				as a data source and may optionally have an object which
 *				acts as a delegate. The data source must adopt the
 *				MBTableGridDataSource protocol, and the delegate must
 *				adopt the MBTableGridDelegate protocol. The data source
 *				provides information that MBTableGrid needs to construct
 *				the table grid and facillitates the insertion, deletion, and
 *				reordering of data within it. The delegate optionally provides
 *				formatting and validation information. For more information
 *				on these, see the MBTableGridDataSource and MBTableGridDelegate
 *				protocols.
 *
 *				MBTableGrid and its methods actually encompass
 *				several subviews, including MBTableGridContentView
 *				(which handles the display, selection, and editing of
 *				cells) and MBTableGridHeaderView (which handles
 *				the display, selection, and dragging of column and
 *				row headers). In general, however, it is not necessary
 *				to interact with these views directly.
 *
 * @author		Matthew Ball
 */
@interface MBTableGrid : NSControl <NSDraggingSource, NSDraggingDestination> {
	/* Headers */
	NSScrollView *columnHeaderScrollView;
	MBTableGridHeaderView *columnHeaderView;
	NSScrollView *rowHeaderScrollView;
	MBTableGridHeaderView *rowHeaderView;
	
	/* Footers */
	NSScrollView *columnFooterScrollView;
	MBTableGridFooterView *columnFooterView;
    NSScrollView *rowFooterScrollView;
    MBTableGridFooterView *rowFooterView;
	
	/* Content */
	NSScrollView *contentScrollView;
	MBTableGridContentView *contentView;

    /* Corners */
    NSVisualEffectView *leadingHeaderCornerView;
    NSVisualEffectView *leadingFooterCornerView;
    NSVisualEffectView *trailingHeaderCornerView;
    NSVisualEffectView *trailingFooterCornerView;
	
	/* Sticky Edges (for Shift+Arrow expansions) */
	MBHorizontalEdge stickyColumnEdge;
	MBVerticalEdge stickyRowEdge;
	NSMutableDictionary<NSNumber *, NSNumber *>* _columnWidths;

    NSTextFinder *_textFinder;
    id<NSTextFinderClient> _textFinderClient;
}

@property (nonatomic, assign) BOOL showsGrabHandles;
@property (getter=isFindBarVisible) BOOL findBarVisible;

@property (getter=isColumnHeaderVisible, nonatomic, assign) BOOL columnHeaderVisible;
@property (getter=isColumnFooterVisible, nonatomic, assign) BOOL columnFooterVisible;
@property (getter=isRowHeaderVisible, nonatomic, assign) BOOL rowHeaderVisible;
@property (getter=isRowHeaderVisible, nonatomic, assign) BOOL rowFooterVisible;

@property (nonatomic, assign) CGFloat rowHeaderWidth;
@property (nonatomic, assign) CGFloat columnFooterHeight;
@property (nonatomic, assign) CGFloat rowFooterWidth;
@property (nonatomic, assign) CGFloat minimumColumnWidth;

@property (nonatomic, readonly) NSView *leadingHeaderCornerView;
@property (nonatomic, readonly) NSView *leadingFooterCornerView;
@property (nonatomic, readonly) NSView *trailingHeaderCornerView;
@property (nonatomic, readonly) NSView *trailingFooterCornerView;

@property (nonatomic) NSEdgeInsets contentInsets;
@property (nonatomic, assign) NSUInteger sortColumnIndex; // NSNotFound for none
@property (getter=isSortColumnAscending, nonatomic, assign) BOOL sortColumnAscending;

- (void)setSelectedRowIndexes:(NSIndexSet *)anIndexSet notify:(BOOL)notify;
- (void)setSelectedColumnIndexes:(NSIndexSet *)anIndexSet notify:(BOOL)notify;

#pragma mark -
#pragma mark Reloading the Grid

/**
 * @name		Reloading the Grid
 */
/**
 * @{
 */

/**
 * @brief		Marks the receiver as needing redisplay, so
 *				it will reload the data for visible cells and
 *				draw the new values.
 *
 * @details		This method forces redraw of all the visible
 *				cells in the receiver. If you want to update
 *				the value in a single cell, column, or row,
 *				it is more efficient to use \c frameOfCellAtColumn:row:,
 *				\c rectOfColumn:, or \c rectOfRow: in conjunction with
 *				\c setNeedsDisplayInRect:.
 *
 * @see			frameOfCellAtColumn:row:
 * @see			rectOfColumn:
 * @see			rectOfRow:
 */
- (void)reloadData;

/**
 * @}
 */

#pragma mark - 
#pragma mark Resize column

/**
 * @name		Resize column
 */
/**
 * @{
 */

/**
 * @brief		Live resizes column
 *
 * @details		This method resizes the column and updates the views
 *
 * @return		The amount that the distance is beyond the minimum size
 *
 */
- (CGFloat)resizeColumnWithIndex:(NSUInteger)columnIndex withDistance:(float)distance location:(NSPoint)location;

/**
 * @brief		Cache of column rects
 *
 * @return		A mutable dictionary containing the records for all the rows keyed by column index number
 *
 */

@property (nonatomic, readonly) NSMutableDictionary<NSNumber *, NSValue *> *columnRects;
@property (nonatomic, readonly) NSMutableDictionary<NSNumber *, NSNumber *> *columnWidths;


- (void)resizeColumnWithIndex:(NSUInteger)columnIndex width:(float)w;

/**
 * @}
 */

#pragma mark -
#pragma mark Selecting Columns and Rows

/**
 * @name		Selecting Columns and Rows
 */
/**
 * @{
 */

/**
 * @brief		Returns an index set containing the indexes of
 *				the selected columns.
 *
 * @return		An index set containing the indexes of the
 *				selected columns.
 *
 * @see			selectedRowIndexes
 * @see			selectCellsInColumns:rows:
 */
@property(nonatomic, strong) NSIndexSet *selectedColumnIndexes;

/**
 * @brief		Returns an index set containing the indexes of
 *				the selected rows.
 *
 * @return		An index set containing the indexes of the
 *				selected rows.
 *
 * @see			selectedColumnIndexes
 * @see			selectCellsInColumns:rows:
 */
@property(nonatomic, strong) NSIndexSet *selectedRowIndexes;

/**
 * @}
 */

#pragma mark -
#pragma mark Dimensions

/**
 * @name		Dimensions
 */
/**
 * @{
 */

/**
 * @brief		Returns the number of rows in the receiver.
 *
 * @return		The number of rows in the receiver.
 *
 * @see			numberOfColumns
 */

@property (nonatomic, assign) NSUInteger numberOfRows;

/**
 * @brief		Returns the number of columns in the receiver.
 *
 * @return		The number of rows in the receiver.
 *
 * @see			numberOfRows
 */

@property (nonatomic, assign) NSUInteger numberOfColumns;


#pragma mark -
#pragma mark Configuring Behavior

/**
 * @name		Configuring Behavior
 */
/**
 * @{
 */

/**
 * @brief		Indicates whether the receiver allows
 *				the user to select more than one cell at
 *				a time.
 *
 * @details		The default is \c YES. You can select multiple
 *				cells programmatically regardless of this setting.
 *
 * @return		\c YES if the receiver allows the user
 *				to select more than one cell at a time.
 *				Otherwise, \c NO.
 */
@property(assign) BOOL allowsMultipleSelection;


/** Whether cells start editing upon first click or upon double click. The default is to edit on double click for most
cells. A cell can individually override this behavior. */
@property (assign) BOOL singleClickCellEdit;

/**
 * @brief		The autosave name for this grid
 */
@property (nonatomic, strong) NSString *autosaveName;

- (void)copy:(id)sender;


/**
 * @}
 */

#pragma mark -
#pragma mark Managing the Delegate and the Data Source
/**
 * @name		Managing the Delegate and the Data Source
 */
/**
 * @{
 */

/**
 * @brief		The object that provides the data displayed by
 *				the grid.
 *
 * @details		The data source must adopt the \c MBTableGridDataSource
 *				protocol. The data source is not retained.
 *
 * @see			delegate
 */
@property(weak) IBOutlet id <MBTableGridDataSource> dataSource;

/**
 * @brief		The object that acts as the delegate of the 
 *				receiving table grid.
 *
 * @details		The delegate must adopt the \c MBTableGridDelegate
 *				protocol. The delegate is not retained.
 *
 * @see			dataSource
 */
@property(nonatomic, weak) IBOutlet id <MBTableGridDelegate> delegate;

/**
 * @}
 */

#pragma mark -
#pragma mark Layout Support
/**
 * @name		Layout Support
 */
/**
 * @{
 */

/**
 * @brief		Returns the rectangle containing the column at
 *				a given index.
 *
 * @param		columnIndex	The index of a column in the receiver.
 *
 * @return		The rectangle containing the column at \c columnIndex.
 *				Returns \c NSZeroRect if \c columnIndex lies outside
 *				the range of valid column indices for the receiver.
 *
 * @see			frameOfCellAtColumn:row:
 * @see			rectOfRow:
 * @see			headerRectOfColumn:
 */
- (NSRect)rectOfColumn:(NSUInteger)columnIndex;

/**
 * @brief		Returns the rectangle containing the row at a
 *				given index.
 *
 * @param		rowIndex	The index of a row in the receiver.
 *
 * @return		The rectangle containing the row at \c rowIndex.
 *				Returns \c NSZeroRect if \c rowIndex lies outside
 *				the range of valid column indices for the receiver.
 *
 * @see			frameOfCellAtColumn:row:
 * @see			rectOfColumn:
 * @see			headerRectOfRow:
 */
- (NSRect)rectOfRow:(NSUInteger)rowIndex;

/**
 * @brief		Returns a rectangle locating the cell that lies at
 *				the intersection of \c columnIndex and \c rowIndex.
 *
 * @param		columnIndex	The index of the column containing the cell
 *							whose rectangle you want.
 * @param		rowIndex	The index of the row containing the cell
 *							whose rectangle you want.
 *
 * @return		A rectangle locating the cell that lies at the intersection
 *				of \c columnIndex and \c rowIndex. Returns \c NSZeroRect if
 *				\c columnIndex or \c rowIndex is greater than the number of
 *				columns or rows in the receiver.
 *
 * @see			rectOfColumn:
 * @see			rectOfRow:
 * @see			headerRectOfColumn:
 * @see			headerRectOfRow:
 */
- (NSRect)frameOfCellAtColumn:(NSUInteger)columnIndex row:(NSUInteger)rowIndex;

/**
 * @brief		Returns the rectangle containing the header tile for
 *				the column at \c columnIndex.
 *
 * @param		columnIndex	The index of the column containing the
 *							header whose rectangle you want.
 *
 * @return		A rectangle locating the header for the column at
 *				\c columnIndex. Returns \c NSZeroRect if \c columnIndex 
 *				lies outside the range of valid column indices for the 
 *				receiver.
 *
 * @see			headerRectOfRow:
 * @see			headerRectOfCorner
 */
- (NSRect)headerRectOfColumn:(NSUInteger)columnIndex;

/**
 * @brief		Returns the rectangle containing the header tile for
 *				the row at \c rowIndex.
 *
 * @param		rowIndex	The index of the row containing the
 *							header whose rectangle you want.
 *
 * @return		A rectangle locating the header for the row at
 *				\c rowIndex. Returns \c NSZeroRect if \c rowIndex 
 *				lies outside the range of valid column indices for the 
 *				receiver.
 *
 * @see			headerRectOfColumn:
 * @see			headerRectOfCorner
 */
- (NSRect)headerRectOfRow:(NSUInteger)rowIndex;

/**
 * @brief		Returns the rectangle containing the corner which
 *				divides the row headers from the column headers.
 *
 * @return		A rectangle locating the corner separating rows
 *				from columns.
 *
 * @see			headerRectOfColumn:
 * @see			headerRectOfRow:
 */
- (NSRect)headerRectOfCorner;

/**
 * @brief		Returns the index of the column a given point lies in.
 *
 * @param		aPoint		A point in the coordinate system of the receiver.
 *
 * @return		The index of the column \c aPoint lies in, or \c NSNotFound if \c aPoint
 *				lies outside the receiver's bounds.
 *
 * @see			rowAtPoint:
 */
- (NSInteger)columnAtPoint:(NSPoint)aPoint;

/**
 * @brief		Returns the index of the row a given point lies in.
 *
 * @param		aPoint		A point in the coordinate system of the receiver.
 *
 * @return		The index of the row \c aPoint lies in, or \c NSNotFound if \c aPoint
 *				lies outside the receiver's bounds.
 *
 * @see			columnAtPoint:
 */
- (NSInteger)rowAtPoint:(NSPoint)aPoint;

/**
 * @}
 */

#pragma mark -
#pragma mark Auxiliary Views
/**
 * @name		Auxiliary Views
 */
/**
 * @{
 */

/**
 * @brief		Returns the \c MBTableGridHeaderView object used
 *				to draw headers over columns.
 *
 * @return		The \c MBTableGridHeaderView object used to draw
 *				column headers.
 *
 * @see			rowHeaderView
 * @see         columnFooterView
 * @see         rowFooterView
 */

@property (nonatomic, readonly) MBTableGridHeaderView* columnHeaderView;

/**
 * @brief        Returns the \c MBTableGridHeaderView object used
 *                to draw headers under columns.
 *
 * @return        The \c MBTableGridHeaderView object used to draw
 *                column footers.
 *
 * @see            rowHeaderView
 * @see            columnHeaderView
 * @see            rowFooterView
 */

@property (nonatomic, readonly) MBTableGridFooterView* columnFooterView;

/**
 * @brief		Returns the \c MBTableGridHeaderView object used
 *				to draw headers beside rows.
 *
 * @return		The \c MBTableGridHeaderView object used to draw
 *				row headers.
 *
 * @see            rowFooterView
 * @see            columnHeaderView
 * @see            columnFooterView
 */

@property (nonatomic, readonly) MBTableGridHeaderView* rowHeaderView;

/**
 * @brief        Returns the \c MBTableGridFooterView object used
 *                to draw footers beside rows.
 *
 * @return        The \c MBTableGridFooterView object used to draw
 *                row footers.
 *
 * @see            rowHeaderView
 * @see            columnHeaderView
 * @see            columnFooterView
 */

@property (nonatomic, readonly) MBTableGridFooterView* rowFooterView;

/**
 * @brief		Returns the receiver's content view.
 *
 * @details		An \c MBTableGrid object uses its content view to
 *				draw the individual cells. It is enclosed in a
 *				scroll view to allow for scrolling.
 *
 * @return		The receiver's content view.
 */
@property (nonatomic, readonly) MBTableGridContentView *contentView;

/**
 * @}
 */

@end

#pragma mark -

/**
 * @brief		The \c MBTableGridDataSource protocol is adopted
 *				by an object that mediates the data model for an
 *				\c MBTableGrid object. 
 *
 * @details		As a representative of the data model, the data 
 *				source supplies no information about the grid's
 *				appearance or behavior. Rather, the grid's
 *				delegate (adopting the \c MBTableGridDelegate
 *				protocol) can provide that information.
 */
@protocol MBTableGridDataSource <NSObject>

@required

#pragma mark -
#pragma mark Dimensions

/**
 * @name		Dimensions
 */
/**
 * @{
 */

@required

/**
 * @brief		Returns the number of rows managed for \c aTableGrid
 *				by the data source object.
 *
 * @param		aTableGrid		The table grid that sent the message.
 *
 * @return		The number of rows in \c aTableGrid.
 *
 * @see			numberOfColumnsInTableGrid:
 */
- (NSUInteger)numberOfRowsInTableGrid:(MBTableGrid *)aTableGrid;

/**
 * @brief		Returns the number of rows managed for \c aTableGrid
 *				by the data source object.
 *
 * @param		aTableGrid		The table grid that sent the message.
 *
 * @return		The number of rows in \c aTableGrid.
 *
 * @see			numberOfRowsInTableGrid:
 */
- (NSUInteger)numberOfColumnsInTableGrid:(MBTableGrid *)aTableGrid;

/**
 * @}
 */

/**
 * @name		Accessing Cell Values
 */
/**
 * @{
 */

@required

/**
 * @brief		Returns the data object associated with the specified column and row.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		columnIndex		A column in \c aTableGrid.
 * @param		rowIndex		A row in \c aTableGrid.
 *
 * @return		The object for the specified cell of the view.
 *
 * @see            tableGrid:setObjectValue:forColumn:row:
 * @see            tableGrid:setObjectValue:forColumns:rows:
 */
- (id) tableGrid:(MBTableGrid *)aTableGrid objectValueForColumn:(NSUInteger)columnIndex row:(NSUInteger)rowIndex;

/**
 * @brief		Returns the celll that should be used to render the specified column and row. The table view should not 
				retain the cell beyond the painting action, as the cell may be re-used to render the next cell.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		columnIndex		A column in \c aTableGrid.
 * @param		rowIndex		A row in \c aTableGrid.
 *
 * @return		The cell to render the specified column.
 */
- (__kindof MBTableGridCell *)tableGrid:(MBTableGrid *)aTableGrid cellForColumn:(NSUInteger)columnIndex row:(NSUInteger)rowIndex;

@optional

/**
 * @brief		Sets the data object for an item in a given row in a given column.
 *
 * @details		Although this method is optional, either it or the plural form \c setObjectValue:forColumns:rows:
 *              must be implemented in order to enable grid editing, Find/Replace, and deletions.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		anObject		The new value for the item, or \c nil for deletions.
 * @param		columnIndex		A column in \c aTableGrid.
 * @param		rowIndex		A row in \c aTableGrid.
 *
 * @see			tableGrid:objectValueForColumn:row:
 * @see         tableGrid:setObjectValueForColumns:rows:

 */
- (void)tableGrid:(MBTableGrid *)aTableGrid setObjectValue:(id)anObject forColumn:(NSUInteger)columnIndex row:(NSUInteger)rowIndex;

@optional

/**
 * @brief        Bulk-replace a single value for all items in the given rows and columns.
 *
 * @details      Although this method is optional, either it or the singular form \c setObjectValue:forColumn:row:
 *               must be implemented in order to enable grid editing, Find/Replace, and deletions.
 *
 * @param        aTableGrid        The table grid that sent the message.
 * @param        anObject        The new value for the items, or \c nil for deletions.
 * @param        columnIndexes        The affected columns in \c aTableGrid.
 * @param        rowIndexes        The affected rows in \c aTableGrid.
 *
 * @see            tableGrid:objectValueForColumn:row:
 * @see            tableGrid:setObjectValue:forColumn:row:
 */
- (void)tableGrid:(MBTableGrid *)aTableGrid setObjectValue:(id)anObject forColumns:(NSIndexSet *)columnIndexes rows:(NSIndexSet *)rowIndexes;


@optional

/**
 * @brief		Sets the column width (in points) for the given column.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		columnIndex		A column in \c aTableGrid.
 *
  * @see			tableGrid:widthForColumn:
 */
- (void) tableGrid:(MBTableGrid *)aTableGrid setWidth:(float)width forColumn:(NSUInteger)columnIndex;

@optional

/**
 * @brief        Returns the column width (in points) for the given column.
 *
 * @param        aTableGrid        The table grid that sent the message.
 * @param        columnIndex        A column in \c aTableGrid.
 *
 * @see          tableGrid:setWidth:forColumn:
 */
- (float) tableGrid:(MBTableGrid *)aTableGrid widthForColumn:(NSUInteger)columnIndex;

/**
 * @}
 */

/**
 * @name		Header Values
 */
/**
 * @{
 */

#pragma mark Header

@optional

/**
 * @brief		Returns the value which should be displayed in the header
 *				for the specified column.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		columnIndex		The index of the column.
 *
 * @return		The header value for the column.
 *
 * @see			tableGrid:headerStringForRow:
 */
- (NSString *)tableGrid:(MBTableGrid *)aTableGrid headerStringForColumn:(NSUInteger)columnIndex;

/**
 * @brief		Returns the value which should be displayed in the header
 *				for the specified row.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		rowIndex		The index of the row.
 *
 * @return		The header value for the row.
 *
 * @see			tableGrid:headerStringForColumn:
 */
- (NSString *)tableGrid:(MBTableGrid *)aTableGrid headerStringForRow:(NSUInteger)rowIndex;

/**
 * @}
 */

#pragma mark Footers

@optional

/**
 *  @brief      Returns the cell for the footer of the specified column.
 *
 * @details		Optional; if not implemented, or returns nil, an empty footer is
 *				displayed for this column.
 *
 *  @param      aTableGrid  The table grid that sent the message.
 *  @param      columnIndex A column in \c aTableGrid.
 *
 *  @return     The cell for the specified column footer.
 */
- (NSCell *)tableGrid:(MBTableGrid *)aTableGrid footerCellForColumn:(NSUInteger)columnIndex;

/**
 *  @brief      Returns the cell for the footer of the specified row.
 *
 * @details        Optional; if not implemented, or returns nil, an empty footer is
 *                displayed for this row.
 *
 *  @param      aTableGrid  The table grid that sent the message.
 *  @param      rowIndex A row in \c aTableGrid.
 *
 *  @return     The cell for the specified row footer.
 */
- (NSCell *)tableGrid:(MBTableGrid *)aTableGrid footerCellForRow:(NSUInteger)rowIndex;


/**
 * @brief		Returns the data object for the footer of the specified column.
 *
 * @details		Optional; if not implemented, or returns nil, an empty footer is
 *				displayed for this column.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		columnIndex		A column in \c aTableGrid.
 *
 * @return		The object for the specified footer of the view.
 *
 * @see			tableGrid:setFooterValue:forColumn:
 */
- (id)tableGrid:(MBTableGrid *)aTableGrid footerValueForColumn:(NSUInteger)columnIndex;

/**
 * @brief		Sets the data object for the footer of the specified column.
 *
 * @details		Optional, but should be implemented for popup-based footer cells.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		anObject		The new value for the item.
 * @param		columnIndex		A column in \c aTableGrid.
 *
 * @see			tableGrid:footerValueForColumn:
 */
- (void)tableGrid:(MBTableGrid *)aTableGrid setFooterValue:(id)anObject forColumn:(NSUInteger)columnIndex;

#pragma mark -
#pragma mark Dragging

/**
 * @name		Dragging
 */
/**
 * @{
 */

@optional

#pragma mark Columns

/**
 * @brief		Returns a Boolean value indicating whether the proposed columns can be
 *				moved to the specified index.
 * 
 * @details		This method is invoked by \c MBTableGrid during drag operations. It allows
 *				the data source to define valid drop targets for columns.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		columnIndexes	An index set describing the columns which
 *								are currently being dragged.
 * @param		index			The proposed index where the columns should
 *								be moved.
 *
 * @return		\c YES if \c columnIndex is a valid drop target, \c NO otherwise.
 *
 * @see			tableGrid:moveColumns:toIndex:
 * @see			tableGrid:canMoveRows:toIndex:
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid canMoveColumns:(NSIndexSet *)columnIndexes toIndex:(NSUInteger)index;

/**
 * @brief		Returns a Boolean value indicating whether the proposed columns
 *				were moved to the specified index.
 *
 * @details		The data source should take care of modifiying the data model to
 *				reflect the changed column order.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		columnIndexes	An index set describing the columns which were dragged.
 * @param		index			The index where the columns should be moved.
 *
 * @return		\c YES if the move was successful, otherwise \c NO.
 *
 * @see			tableGrid:canMoveColumns:toIndex:
 * @see			tableGrid:moveRows:toIndex:
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid moveColumns:(NSIndexSet *)columnIndexes toIndex:(NSUInteger)index;

#pragma mark Rows

/**
 * @brief		Returns a Boolean value indicating whether the proposed rows can be
 *				moved to the specified index.
 * 
 * @details		This method is invoked by \c MBTableGrid during drag operations. It allows
 *				the data source to define valid drop targets for rows.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		rowIndexes		An index set describing the rows which
 *								are currently being dragged.
 * @param		index			The proposed index where the rows should
 *								be moved.
 *
 * @return		\c YES if \c rowIndex is a valid drop target, \c NO otherwise.
 *
 * @see			tableGrid:moveRows:toIndex:
 * @see			tableGrid:canMoveColumns:toIndex:
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid canMoveRows:(NSIndexSet *)rowIndexes toIndex:(NSUInteger)index;

/**
 * @brief		Returns a Boolean value indicating whether the proposed rows
 *				were moved to the specified index.
 *
 * @details		The data source should take care of modifiying the data model to
 *				reflect the changed row order.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		rowIndexes		An index set describing the rows which were dragged.
 * @param		index			The index where the rows should be moved.
 *
 * @return		\c YES if the move was successful, otherwise \c NO.
 *
 * @see			tableGrid:canMoveRows:toIndex:
 * @see			tableGrid:moveColumns:toIndex:
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid moveRows:(NSIndexSet *)rowIndexes toIndex:(NSUInteger)index;

#pragma mark Other Values

/**
 * @brief		Used by \c aTableGrid to determine a valid drop target.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		info			An object that contains more information about
 *								this dragging operation.
 * @param		columnIndex		The index of the proposed target column.
 * @param		rowIndex		The index of the proposed target row.
 *
 * @return		The dragging operation the data source will perform.
 *
 * @see			tableGrid:acceptDrop:column:row:
 */
- (NSDragOperation)tableGrid:(MBTableGrid *)aTableGrid validateDrop:(id <NSDraggingInfo>)info proposedColumn:(NSUInteger)columnIndex row:(NSUInteger)rowIndex;

/**
 * @brief		Invoked by \c aTableGrid when the mouse button is released over
 *				a table grid that previously decided to allow a drop.
 *
 * @details		The data source should incorporate the data from the dragging
 *				pasteboard in the implementation of this method. You can get the
 *				data for the drop operation from \c info using the \c draggingPasteboard
 *				method.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		info			An object that contains more information about
 *								this dragging operation.
 * @param		columnIndex		The index of the proposed target column.
 * @param		rowIndex		The index of the proposed target row.
 *
 * @return		\c YES if the drop was successful, otherwise \c NO.
 *
 * @see			tableGrid:validateDrop:proposedColumn:row:
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid acceptDrop:(id <NSDraggingInfo>)info column:(NSUInteger)columnIndex row:(NSUInteger)rowIndex;

#pragma mark -
#pragma mark Adding and Removing Rows

@optional

/**
 * @brief		Returns a Boolean value indicating whether the rows
 *				were successfully added.
 *
 * @details		The data source should take care of modifiying the data model to
 *				add the rows.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		numberOfRows	The number of rows to add.
 *
 * @return		\c YES if the add was successful, otherwise \c NO.
 *
 * @see			tableGrid:removeRows:
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid addRows:(NSUInteger)numberOfRows;

/**
 * @brief		Returns a Boolean value indicating whether the specified rows
 *				were removed.
 *
 * @details		The data source should take care of modifiying the data model to
 *				reflect the removed rows.
 *
 * @param		aTableGrid		The table grid that sent the message.
 * @param		rowIndexes		An index set describing the rows to remove.
 *
 * @return		\c YES if the removal was successful, otherwise \c NO.
 *
 * @see			tableGrid:addRows:
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid removeRows:(NSIndexSet *)rowIndexes;


#pragma mark Sorting

/**
 * @brief        Returns an index set indicating which columns in the table grid can be sorted.
 *
 * @details      The data source is responsible for sorting the data. The current sort specification
 *               can be accessed via the table grid's \c sortColumnIndex and \c isSortColumnAscending
 *               properties.
 *
 * @param        aTableGrid        The table grid that sent the message.
 *
 * @return       The index set of columns that will have a sort indicator appear in the table grid's
 *              colunm header, or \c nil if no  if no columns should be sortable.
 *
 * @see          tableGrid:didSortByColumn:ascending:
*/

- (NSIndexSet *)sortableColumnIndexesInTableGrid:(MBTableGrid*)aTableGrid;

/**
 * @}
 */

@end

#pragma mark -

/**
 * @brief		The delegate of an \c MBTableGrid object must adopt the
 *				\c MBTableGridDelegate protocol. Optional methods of the
 *				protocol allow the delegate to validate selections and data
 *				modifications and provide formatting information.
 */
@protocol MBTableGridDelegate <NSObject>

// Being a delegate, the entire protocol is optional
@optional

#pragma mark -
#pragma mark Managing Selections
/**
 * @name		Managing Selections
 */
/**
 * @{
 */

/**
 * @brief		Tells the delegate that the specified columns are about to be
 *				selected.
 *
 * @param		aTableGrid		The table grid object informing the delegate
 *								about the impending selection.
 * @param		indexPath		An index path locating the columns in \c aTableGrid.
 *
 * @return		An index path which confirms or alters the impending selection.
 *				Return an \c NSIndexPath object other than \c indexPath if you want
 *				different columns to be selected.
 *
 * @see			tableGrid:willSelectRowsAtIndexPath:
 */
- (NSIndexSet *)tableGrid:(MBTableGrid *)aTableGrid willSelectColumnsAtIndexPath:(NSIndexSet *)indexPath;

/**
 * @brief		Tells the delegate that the specified rows are about to be
 *				selected.
 *
 * @param		aTableGrid		The table grid object informing the delegate
 *								about the impending selection.
 * @param		indexPath		An index path locating the rows in \c aTableGrid.
 *
 * @return		An index path which confirms or alters the impending selection.
 *				Return an \c NSIndexPath object other than \c indexPath if you want
 *				different rows to be selected.
 *
 * @see			tableGrid:willSelectColumnsAtIndexPath:
 */
- (NSIndexSet *)tableGrid:(MBTableGrid *)aTableGrid willSelectRowsAtIndexPath:(NSIndexSet *)indexPath;

/**
 * @brief		Informs the delegate that the table grid's selection has changed.
 *
 * @details		\c aNotification is an \c MBTableGridDidChangeSelectionNotification.
 */
- (void)tableGridDidChangeSelection:(NSNotification *)aNotification;

/**
 * @brief		Tells the delegate that the specified column header was double-clicked
 *
 * @param		aTableGrid		The table grid object informing the delegate
 *								about the double-click event
 * @param		columnIndex		The double-clicked column in \c aTableGrid.
 *
 *
 * @see			tableGrid:didDoubleClickRow:
 */
- (void)tableGrid:(MBTableGrid *)aTableGrid didDoubleClickColumn:(NSUInteger)columnIndex;

/**
 * @brief        Tells the delegate that the specified row header was double-clicked
 *
 * @param        aTableGrid        The table grid object informing the delegate
 *                                about the double-click event
 * @param        rowIndex        The double-clicked row in \c aTableGrid.
 *
 *
 * @see            tableGrid:didDoubleClickColumn:
 */
- (void)tableGrid:(MBTableGrid *)aTableGrid didDoubleClickRow:(NSUInteger)rowIndex;

/**
 * @brief        Tells the delegate that the specified cell was double-clicked
 *
 * @param        aTableGrid        The table grid object informing the delegate
 *                                about the double-click event
 * @param        columnIndex        The double-clicked column in \c aTableGrid.
 * @param        rowIndex        The double-clicked column in \c aTableGrid.
 *
 *
 * @see            tableGrid:didDoubleClickColumn:
*/
- (void)tableGrid:(MBTableGrid *)aTableGrid didDoubleClickColumn:(NSUInteger)columnIndex row:(NSUInteger)rowIndex;

#pragma mark -
#pragma mark Find Bar Support

/**
 * @brief       Returns a boolean indicating whether the specified text finder action should be allowed.
 *
 * @param       aTableGrid      The table grid object that sent the message
 * @param       finderActionTag The proposed finder action
 *
 * @see         NSTextFinderAction
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid validateTextFinderAction:(NSTextFinderAction)finderActionTag;

/**
 * @brief       Tells the delegate that the specified text finder action was performed.
 *
 * @param       aTableGrid      The table grid object that sent the message
 * @param       finderActionTag The performed finder action
 *
 * @see         NSTextFinderAction
 */
- (void)tableGrid:(MBTableGrid *)aTableGrid didPerformTextFinderAction:(NSTextFinderAction)finderActionTag;

/**
 * @}
 */

#pragma mark -
#pragma mark Moving Columns and Rows

/**
 * @name		Moving Columns and Rows
 */
/**
 * @{
 */

/**
 * @brief		Informs the delegate that columns were moved by user action in
 *				the table grid.
 *
 * @details		\c aNotification is an \c MBTableGridDidMoveColumnsNotification.
 *
 * @see			tableGridDidMoveRows:
 */
- (void)tableGridDidMoveColumns:(NSNotification *)aNotification;

/**
 * @brief		Informs the delegate that rows were moved by user action in
 *				the table grid.
 *
 * @details		\c aNotification is an \c MBTableGridDidMoveRowsNotification.
 *
 * @see			tableGridDidMoveColumns:
 */
- (void)tableGridDidMoveRows:(NSNotification *)aNotification;

/**
 * @}
 */

#pragma mark -
#pragma mark Editing Cells

/**
 * @name		Editing Cells
 */
/**
 * @{
 */

/**
 * @brief		Asks the delegate if the specified cell can be edited.
 *
 * @details		The delegate can implement this method to disallow editing of
 *				specific cells.
 *
 * @param		aTableGrid		The table grid which will edit the cell.
 * @param		columnIndex		The column of the cell.
 * @param		rowIndex		The row of the cell.
 *
 * @return		\c YES to permit \c aTableGrid to edit the specified cell, \c NO to deny permission.
 */
- (BOOL)tableGrid:(MBTableGrid *)aTableGrid shouldEditColumn:(NSUInteger)columnIndex row:(NSUInteger)rowIndex;

/**
 *  @brief      Informs the delegate of the cells that should be copied to the clipboard.
 *
 *  @param      aTableGrid       The table grid that contains the cell.
 *  @param      columnIndexes    Column indexes of the cells being copied.
 *  @param      rowIndexes       Row indexes of the cells being copied.
 */
- (void)tableGrid:(MBTableGrid *)aTableGrid copyCellsAtColumns:(NSIndexSet *)columnIndexes rows:(NSIndexSet *)rowIndexes;

/**
 *  @brief      Informs the delegate of the cells that should be pasted from the clipboard.
 *
 *  @param      aTableGrid       The table grid that contains the cell.
 *  @param      columnIndexes    Column indexes of the cells being copied.
 *  @param      rowIndexes       Row indexes of the cells being copied.
 */
- (void)tableGrid:(MBTableGrid *)aTableGrid pasteCellsAtColumns:(NSIndexSet *)columnIndexes rows:(NSIndexSet *)rowIndexes;

/**
 * @}
 */

#pragma mark Adding Rows

/**
 * @brief		Tells the delegate that the specified rows were added.
 *
 * @param		aTableGrid		The table grid object informing the delegate
 *								about the added rows.
 * @param		rowIndexes		An index set describing the rows that were added.
 */
- (void)tableGrid:(MBTableGrid *)aTableGrid didAddRows:(NSIndexSet *)rowIndexes;

- (void)tableGrid:(MBTableGrid*)aTableGrid footerCellClicked:(NSCell*)cell forColumn:(NSUInteger)columnIndex withEvent:(NSEvent*)theEvent;

- (void)tableGrid:(MBTableGrid*)aTableGrid footerCellClicked:(NSCell*)cell forRow:(NSUInteger)columnIndex withEvent:(NSEvent*)theEvent;

#pragma mark Sorting

/**
 * @brief        Tells the delegate that the table grid's sort specification has changed.
 *
 * @param        aTableGrid        The table grid that sent the message.
 * @param        columnIndex      The column that should be sorted by, or \c NSNotFound if no sorting should occur.
 * @param        isAscending      \c YES if the items should be sorted in ascending order; \c NO otherwise
 *
*/

- (void)tableGrid:(MBTableGrid*)aTableGrid didSortByColumn:(NSUInteger)columnIndex ascending:(BOOL)isAscending;

#pragma mark Displaying Menus

/**
 * @brief   Informs the delegate that the column header's contextual menu is about to be displayed.
 *          The menu items can then be customized for the particular column.
 *          (The column header menu can be set via the \c menu property of the table grid's \c columnHeaderView.)
 *
 *  @param  aTableGrid      The table grid object that will display the menu
 *
 *  @param  menu                    The menu about to be displayed
 *
 *  @param  columnIndex     The column that was clicked (right-clicked or Control-clicked)
 *
 *  @see    tableGrid:willDisplayFooterMenu:forColumn:
 *  @see    tableGrid:willDisplayHeaderMenu:forRow:
 *  @see    tableGrid:willDisplayFooterMenu:forRow:
 */

- (void)tableGrid:(MBTableGrid *)aTableGrid willDisplayHeaderMenu:(NSMenu *)menu forColumn:(NSUInteger)columnIndex;

/**
 * @brief   Informs the delegate that the column footer's contextual menu is about to be displayed.
 *          The menu items can then be customized for the particular column.
 *          (The column footer menu can be set via the \c menu property of the table grid's \c columnFooterView.)
 *
 *  @param  aTableGrid      The table grid object that will display the menu
 *
 *  @param  menu                    The menu about to be displayed
 *
 *  @param  columnIndex     The column that was clicked (right-clicked or Control-clicked)
 *
 *  @see    tableGrid:willDisplayHeaderMenu:forRow:
 *  @see    tableGrid:willDisplayHeaderMenu:forColumn:
 *  @see    tableGrid:willDisplayFooterMenu:forRow:
 */

- (void)tableGrid:(MBTableGrid *)aTableGrid willDisplayFooterMenu:(NSMenu *)menu forColumn:(NSUInteger)columnIndex;

/**
 * @brief   Informs the delegate that the row header's contextual menu is about to be displayed.
 *          The menu items can then be customized for the particular row.
 *          (The row header menu can be set via the \c menu property of the table grid's \c rowHeaderView.)
 *
 *  @param  aTableGrid      The table grid object that will display the menu
 *
 *  @param  menu                    The menu about to be displayed
 *
 *  @param  rowIndex            The row that was clicked (right-clicked or Control-clicked)
 *
 *  @see    tableGrid:willDisplayHeaderMenu:forColumn:
 *  @see    tableGrid:willDisplayFooterMenu:forColumn:
 *  @see    tableGrid:willDisplayFooterMenu:forRow:
 */

- (void)tableGrid:(MBTableGrid *)aTableGrid willDisplayHeaderMenu:(NSMenu *)menu forRow:(NSUInteger)rowIndex;

/**
 * @brief   Informs the delegate that the row footer's contextual menu is about to be displayed.
 *          The menu items can then be customized for the particular row.
 *          (The row footer menu can be set via the \c menu property of the table grid's \c rowFooterView.)
 *
 *  @param  aTableGrid      The table grid object that will display the menu
 *
 *  @param  menu                    The menu about to be displayed
 *
 *  @param  rowIndex            The row that was clicked (right-clicked or Control-clicked)
 *
 *  @see    tableGrid:willDisplayHeaderMenu:forColumn:
 *  @see    tableGrid:willDisplayHeaderMenu:forRow:
 *  @see    tableGrid:willDisplayFooterMenu:forColumn:
 */

- (void)tableGrid:(MBTableGrid *)aTableGrid willDisplayFooterMenu:(NSMenu *)menu forRow:(NSUInteger)rowIndex;

@end
