#ifndef E_REPORT_INTERFACE_H
#define E_REPORT_INTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

void initExtReport(void);
void ExtReport(void);
void ExtReportToJsonFile(void);

#ifdef __cplusplus
}
#endif

#endif // E_REPORT_INTERFACE_H
