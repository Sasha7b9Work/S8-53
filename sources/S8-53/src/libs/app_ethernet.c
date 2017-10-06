#include "main.h"
#include "lwip/dhcp.h"
#include "app_ethernet.h"
#ifdef USE_LCD
#include "lcd_log.h"
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_DHCP
#define MAX_DHCP_TRIES  4
uint32_t DHCPfineTimer = 0;
__IO uint8_t DHCP_state = DHCP_OFF;
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  Notify the User about the nework interface config status 
  * @param  netif: the network interface
  */
void User_notification(struct netif *netif) 
{
#ifdef USE_DHCP
    if (netif_is_up(netif))
    {
        /* Update DHCP state machine */
        DHCP_state = DHCP_START;
    }
    else
    {  
        /* Update DHCP state machine */
        DHCP_state = DHCP_LINK_DOWN;
    } 
#endif  /* USE_DHCP */
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  */
void ethernetif_notify_conn_changed(struct netif *netif)
{
#ifndef USE_DHCP
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
#endif
  
    if(netif_is_link_up(netif))
    {
#ifdef USE_DHCP
        /* Update DHCP state machine */
        DHCP_state = DHCP_START;
#else
        IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
        IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);  
    
        netif_set_addr(netif, &ipaddr , &netmask, &gw);  
    
#ifdef USE_LCD        
        uint8_t iptxt[20];
        sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
        LCD_UsrLog ("Static IP address: %s\n", iptxt);
#endif
#endif /* USE_DHCP */   
    
        /* When the netif is fully configured this function must be called.*/
        netif_set_up(netif);     
    }
    else
    {
#ifdef USE_DHCP
        /* Update DHCP state machine */
        DHCP_state = DHCP_LINK_DOWN;
#endif /* USE_DHCP */
    
        /*  When the netif link is down this function must be called.*/
        netif_set_down(netif);
  }
}

#ifdef USE_DHCP
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
  * @brief  DHCP_Process_Handle
  */
void DHCP_Process(struct netif *netif)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    struct dhcp *dhcp;   
  
    switch (DHCP_state)
    {
        case DHCP_START:
        {
            ip_addr_set_zero_ip4(&netif->ip_addr);
            ip_addr_set_zero_ip4(&netif->netmask);
            ip_addr_set_zero_ip4(&netif->gw);
            DHCP_state = DHCP_WAIT_ADDRESS;
            dhcp_start(netif);
        }
        break;
    
        case DHCP_WAIT_ADDRESS:
        {
            if (dhcp_supplied_address(netif)) 
            {
                DHCP_state = DHCP_ADDRESS_ASSIGNED;
            }
            else
            {
                dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
        
                /* DHCP timeout */
                if (dhcp->tries > MAX_DHCP_TRIES)
                {
                    DHCP_state = DHCP_TIMEOUT;
                    
                    /* Stop DHCP */
                    dhcp_stop(netif);
                    
                    /* Static address used */
                    IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
                    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                    netif_set_addr(netif, &ipaddr, &netmask, &gw);
                }
            }
        }
        break;
    case DHCP_LINK_DOWN:
        {
            /* Stop DHCP */
            dhcp_stop(netif);
            DHCP_state = DHCP_OFF; 
        }
        break;
    default: break;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
  * @brief  DHCP periodic check
  * @param  localtime the current LocalTime value
  */
void DHCP_Periodic_Handle(struct netif *netif)
{  
    /* Fine DHCP periodic process every 500ms */
    if (HAL_GetTick() - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
    {
        DHCPfineTimer =  HAL_GetTick();
        /* process DHCP state machine */
        DHCP_Process(netif);    
    }
}
#endif
