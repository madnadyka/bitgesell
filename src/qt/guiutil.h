// Copyright (c) 2011-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BGL_QT_GUIUTIL_H
#define BGL_QT_GUIUTIL_H

#include <amount.h>
#include <fs.h>

#include <QEvent>
#include <QHeaderView>
#include <QItemDelegate>
#include <QMessageBox>
#include <QObject>
#include <QProgressBar>
#include <QString>
#include <QTableView>
#include <QLabel>

class QValidatedLineEdit;
class SendCoinsRecipient;

namespace interfaces
{
    class Node;
}

QT_BEGIN_NAMESPACE
class QAbstractItemView;
class QAction;
class QDateTime;
class QFont;
class QLineEdit;
class QMenu;
class QPoint;
class QProgressDialog;
class QUrl;
class QWidget;
QT_END_NAMESPACE

/** Utility functions used by the BGL Qt UI.
 */
namespace GUIUtil
{
    // Create human-readable string from date
    QString dateTimeStr(const QDateTime &datetime);
    QString dateTimeStr(qint64 nTime);

    // Return a monospace font
    QFont fixedPitchFont();

    // Set up widget for address
    void setupAddressWidget(QValidatedLineEdit *widget, QWidget *parent);

    // Parse "BGL:" URI into recipient object, return true on successful parsing
    bool parseBGLURI(const QUrl &uri, SendCoinsRecipient *out);
    bool parseBGLURI(QString uri, SendCoinsRecipient *out);
    QString formatBGLURI(const SendCoinsRecipient &info);

    // Returns true if given address+amount meets "dust" definition
    bool isDust(interfaces::Node& node, const QString& address, const CAmount& amount);

    // HTML escaping for rich text controls
    QString HtmlEscape(const QString& str, bool fMultiLine=false);
    QString HtmlEscape(const std::string& str, bool fMultiLine=false);

    /** Copy a field of the currently selected entry of a view to the clipboard. Does nothing if nothing
        is selected.
       @param[in] column  Data column to extract from the model
       @param[in] role    Data role to extract from the model
       @see  TransactionView::copyLabel, TransactionView::copyAmount, TransactionView::copyAddress
     */
    void copyEntryData(const QAbstractItemView *view, int column, int role=Qt::EditRole);

    /** Return a field of the currently selected entry as a QString. Does nothing if nothing
        is selected.
       @param[in] column  Data column to extract from the model
       @see  TransactionView::copyLabel, TransactionView::copyAmount, TransactionView::copyAddress
     */
    QList<QModelIndex> getEntryData(const QAbstractItemView *view, int column);

    /** Returns true if the specified field of the currently selected view entry is not empty.
       @param[in] column  Data column to extract from the model
       @param[in] role    Data role to extract from the model
       @see  TransactionView::contextualMenu
     */
    bool hasEntryData(const QAbstractItemView *view, int column, int role);

    void setClipboard(const QString& str);

    /**
     * Determine default data directory for operating system.
     */
    QString getDefaultDataDirectory();

    /** Get save filename, mimics QFileDialog::getSaveFileName, except that it appends a default suffix
        when no suffix is provided by the user.

      @param[in] parent  Parent window (or 0)
      @param[in] caption Window caption (or empty, for default)
      @param[in] dir     Starting directory (or empty, to default to documents directory)
      @param[in] filter  Filter specification such as "Comma Separated Files (*.csv)"
      @param[out] selectedSuffixOut  Pointer to return the suffix (file type) that was selected (or 0).
                  Can be useful when choosing the save file format based on suffix.
     */
    QString getSaveFileName(QWidget *parent, const QString &caption, const QString &dir,
        const QString &filter,
        QString *selectedSuffixOut);

    /** Get open filename, convenience wrapper for QFileDialog::getOpenFileName.

      @param[in] parent  Parent window (or 0)
      @param[in] caption Window caption (or empty, for default)
      @param[in] dir     Starting directory (or empty, to default to documents directory)
      @param[in] filter  Filter specification such as "Comma Separated Files (*.csv)"
      @param[out] selectedSuffixOut  Pointer to return the suffix (file type) that was selected (or 0).
                  Can be useful when choosing the save file format based on suffix.
     */
    QString getOpenFileName(QWidget *parent, const QString &caption, const QString &dir,
        const QString &filter,
        QString *selectedSuffixOut);

    /** Get connection type to call object slot in GUI thread with invokeMethod. The call will be blocking.

       @returns If called from the GUI thread, return a Qt::DirectConnection.
                If called from another thread, return a Qt::BlockingQueuedConnection.
    */
    Qt::ConnectionType blockingGUIThreadConnection();

    // Determine whether a widget is hidden behind other windows
    bool isObscured(QWidget *w);

    // Activate, show and raise the widget
    void bringToFront(QWidget* w);

    // Set shortcut to close window
    void handleCloseWindowShortcut(QWidget* w);

    // Open debug.log
    void openDebugLogfile();

    // Open the config file
    bool openBGLConf();

    /** Qt event filter that intercepts ToolTipChange events, and replaces the tooltip with a rich text
      representation if needed. This assures that Qt can word-wrap long tooltip messages.
      Tooltips longer than the provided size threshold (in characters) are wrapped.
     */
    class ToolTipToRichTextFilter : public QObject
    {
        Q_OBJECT

    public:
        explicit ToolTipToRichTextFilter(int size_threshold, QObject *parent = nullptr);

    protected:
        bool eventFilter(QObject *obj, QEvent *evt) override;

    private:
        int size_threshold;
    };

    /**
     * Qt event filter that intercepts QEvent::FocusOut events for QLabel objects, and
     * resets their `textInteractionFlags' property to get rid of the visible cursor.
     *
     * This is a temporary fix of QTBUG-59514.
     */
    class LabelOutOfFocusEventFilter : public QObject
    {
        Q_OBJECT

    public:
        explicit LabelOutOfFocusEventFilter(QObject* parent);
        bool eventFilter(QObject* watched, QEvent* event) override;
    };

    /**
     * Makes a QTableView last column feel as if it was being resized from its left border.
     * Also makes sure the column widths are never larger than the table's viewport.
     * In Qt, all columns are resizable from the right, but it's not intuitive resizing the last column from the right.
     * Usually our second to last columns behave as if stretched, and when on stretch mode, columns aren't resizable
     * interactively or programmatically.
     *
     * This helper object takes care of this issue.
     *
     */
    class TableViewLastColumnResizingFixer: public QObject
    {
        Q_OBJECT

        public:
            TableViewLastColumnResizingFixer(QTableView* table, int lastColMinimumWidth, int allColsMinimumWidth, QObject *parent);
            void stretchColumnWidth(int column);

        private:
            QTableView* tableView;
            int lastColumnMinimumWidth;
            int allColumnsMinimumWidth;
            int lastColumnIndex;
            int columnCount;
            int secondToLastColumnIndex;

            void adjustTableColumnsWidth();
            int getAvailableWidthForColumn(int column);
            int getColumnsWidth();
            void connectViewHeadersSignals();
            void disconnectViewHeadersSignals();
            void setViewHeaderResizeMode(int logicalIndex, QHeaderView::ResizeMode resizeMode);
            void resizeColumn(int nColumnIndex, int width);

        private Q_SLOTS:
            void on_sectionResized(int logicalIndex, int oldSize, int newSize);
            void on_geometriesChanged();
    };

    bool GetStartOnSystemStartup();
    bool SetStartOnSystemStartup(bool fAutoStart);

    /* Convert QString to OS specific boost path through UTF-8 */
    fs::path qstringToBoostPath(const QString &path);

    /* Convert OS specific boost path to QString through UTF-8 */
    QString boostPathToQString(const fs::path &path);

    /* Convert seconds into a QString with days, hours, mins, secs */
    QString formatDurationStr(int secs);

    /* Format CNodeStats.nServices bitmask into a user-readable string */
    QString formatServicesStr(quint64 mask);

    /* Format a CNodeStats.m_ping_usec into a user-readable string or display N/A, if 0*/
    QString formatPingTime(int64_t ping_usec);

    /* Format a CNodeCombinedStats.nTimeOffset into a user-readable string. */
    QString formatTimeOffset(int64_t nTimeOffset);

    QString formatNiceTimeOffset(qint64 secs);

    QString formatBytes(uint64_t bytes);

    qreal calculateIdealFontSize(int width, const QString& text, QFont font, qreal minPointSize = 4, qreal startPointSize = 14);

    class ClickableLabel : public QLabel
    {
        Q_OBJECT

    Q_SIGNALS:
        /** Emitted when the label is clicked. The relative mouse coordinates of the click are
         * passed to the signal.
         */
        void clicked(const QPoint& point);
    protected:
        void mouseReleaseEvent(QMouseEvent *event) override;
    };

    class ClickableProgressBar : public QProgressBar
    {
        Q_OBJECT

    Q_SIGNALS:
        /** Emitted when the progressbar is clicked. The relative mouse coordinates of the click are
         * passed to the signal.
         */
        void clicked(const QPoint& point);
    protected:
        void mouseReleaseEvent(QMouseEvent *event) override;
    };

    typedef ClickableProgressBar ProgressBar;

    class ItemDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        ItemDelegate(QObject* parent) : QItemDelegate(parent) {}

    Q_SIGNALS:
        void keyEscapePressed();

    private:
        bool eventFilter(QObject *object, QEvent *event) override;
    };

    // Fix known bugs in QProgressDialog class.
    void PolishProgressDialog(QProgressDialog* dialog);

    /**
     * Returns the distance in pixels appropriate for drawing a subsequent character after text.
     *
     * In Qt 5.12 and before the QFontMetrics::width() is used and it is deprecated since Qt 13.0.
     * In Qt 5.11 the QFontMetrics::horizontalAdvance() was introduced.
     */
    int TextWidth(const QFontMetrics& fm, const QString& text);

    /**
     * Writes to debug.log short info about the used Qt and the host system.
     */
    void LogQtInfo();

    /**
     * Call QMenu::popup() only on supported QT_QPA_PLATFORM.
     */
    void PopupMenu(QMenu* menu, const QPoint& point, QAction* at_action = nullptr);

    /**
     * Returns the start-moment of the day in local time.
     *
     * QDateTime::QDateTime(const QDate& date) is deprecated since Qt 5.15.
     * QDate::startOfDay() was introduced in Qt 5.14.
     */
    QDateTime StartOfDay(const QDate& date);

    /**
     * Returns true if pixmap has been set.
     *
     * QPixmap* QLabel::pixmap() is deprecated since Qt 5.15.
     */
    bool HasPixmap(const QLabel* label);
    QImage GetImage(const QLabel* label);

    /**
     * Splits the string into substrings wherever separator occurs, and returns
     * the list of those strings. Empty strings do not appear in the result.
     *
     * QString::split() signature differs in different Qt versions:
     *  - QString::SplitBehavior is deprecated since Qt 5.15
     *  - Qt::SplitBehavior was introduced in Qt 5.14
     * If {QString|Qt}::SkipEmptyParts behavior is required, use this
     * function instead of QString::split().
     */
    template <typename SeparatorType>
    QStringList SplitSkipEmptyParts(const QString& string, const SeparatorType& separator)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        return string.split(separator, Qt::SkipEmptyParts);
    #else
        return string.split(separator, QString::SkipEmptyParts);
    #endif
    }

    /**
     * Queue a function to run in an object's event loop. This can be
     * replaced by a call to the QMetaObject::invokeMethod functor overload after Qt 5.10, but
     * for now use a QObject::connect for compatibility with older Qt versions, based on
     * https://stackoverflow.com/questions/21646467/how-to-execute-a-functor-or-a-lambda-in-a-given-thread-in-qt-gcd-style
     */
    template <typename Fn>
    void ObjectInvoke(QObject* object, Fn&& function, Qt::ConnectionType connection = Qt::QueuedConnection)
    {
        QObject source;
        QObject::connect(&source, &QObject::destroyed, object, std::forward<Fn>(function), connection);
    }

} // namespace GUIUtil

#endif // BGL_QT_GUIUTIL_H
