/* -*- mode: c; c-basic-offset: 8; indent-tabs-mode: nil; -*-
 * vim:expandtab:shiftwidth=8:tabstop=8:
 *
 *  Copyright (C) 2001, 2002 Cluster File Systems, Inc.
 *   Author: Peter J. Braam <braam@clusterfs.com>
 *   Author: Phil Schwan <phil@clusterfs.com>
 *
 *   This file is part of Lustre, http://www.lustre.org.
 *
 *   Lustre is free software; you can redistribute it and/or
 *   modify it under the terms of version 2 of the GNU General Public
 *   License as published by the Free Software Foundation.
 *
 *   Lustre is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Lustre; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Storage Target Handling functions
 *  Lustre Object Server Module (OST)
 *
 *  This server is single threaded at present (but can easily be multi
 *  threaded). For testing and management it is treated as an
 *  obd_device, although it does not export a full OBD method table
 *  (the requests are coming in over the wire, so object target
 *  modules do not have a full method table.)
 */

#define EXPORT_SYMTAB

#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/locks.h>
#include <linux/ext2_fs.h>
#include <linux/quotaops.h>
#include <asm/unistd.h>

#define DEBUG_SUBSYSTEM S_OST

#include <linux/obd_ost.h>
#include <linux/lustre_net.h>

static int ost_destroy(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = ost->ost_tgt;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }

        req->rq_rep.ost->result = obd_destroy(&conn, &req->rq_req.ost->oa);

        RETURN(0);
}

static int ost_getattr(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = ost->ost_tgt;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }
        req->rq_rep.ost->oa.o_id = req->rq_req.ost->oa.o_id;
        req->rq_rep.ost->oa.o_valid = req->rq_req.ost->oa.o_valid;

        req->rq_rep.ost->result =  obd_getattr(&conn, &req->rq_rep.ost->oa);

        RETURN(0);
}

static int ost_open(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = ost->ost_tgt;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }
        req->rq_rep.ost->oa.o_id = req->rq_req.ost->oa.o_id;
        req->rq_rep.ost->oa.o_valid = req->rq_req.ost->oa.o_valid;

        req->rq_rep.ost->result =  obd_open(&conn, &req->rq_rep.ost->oa);

        RETURN(0);
}

static int ost_close(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = ost->ost_tgt;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }
        req->rq_rep.ost->oa.o_id = req->rq_req.ost->oa.o_id;
        req->rq_rep.ost->oa.o_valid = req->rq_req.ost->oa.o_valid;

        req->rq_rep.ost->result =  obd_close(&conn, &req->rq_rep.ost->oa);

        RETURN(0);
}


static int ost_create(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = ost->ost_tgt;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }

        memcpy(&req->rq_rep.ost->oa, &req->rq_req.ost->oa,
               sizeof(req->rq_req.ost->oa));

        req->rq_rep.ost->result =obd_create(&conn, &req->rq_rep.ost->oa);

        RETURN(0);
}

static int ost_punch(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = ost->ost_tgt;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }

        memcpy(&req->rq_rep.ost->oa, &req->rq_req.ost->oa,
               sizeof(req->rq_req.ost->oa));

        req->rq_rep.ost->result = obd_punch(&conn, &req->rq_rep.ost->oa,
                                            req->rq_rep.ost->oa.o_size,
                                            req->rq_rep.ost->oa.o_blocks);

        RETURN(0);
}


static int ost_setattr(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = ost->ost_tgt;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }

        memcpy(&req->rq_rep.ost->oa, &req->rq_req.ost->oa,
               sizeof(req->rq_req.ost->oa));

        req->rq_rep.ost->result = obd_setattr(&conn, &req->rq_rep.ost->oa);

        RETURN(0);
}

static int ost_connect(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_dev = ost->ost_tgt;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }

        req->rq_rep.ost->result = obd_connect(&conn);

        CDEBUG(D_IOCTL, "rep buffer %p, id %d\n", req->rq_repbuf, conn.oc_id);
        req->rq_rep.ost->connid = conn.oc_id;
        RETURN(0);
}

static int ost_disconnect(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;

        ENTRY;

        conn.oc_dev = ost->ost_tgt;
        conn.oc_id = req->rq_req.ost->connid;

        rc = ost_pack_rep(NULL, 0, NULL, 0, &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }
        CDEBUG(D_IOCTL, "Disconnecting %d\n", conn.oc_id);
        req->rq_rep.ost->result = obd_disconnect(&conn);

        RETURN(0);
}

static int ost_get_info(struct ost_obd *ost, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;
        int vallen;
        void *val;
        char *ptr;

        ENTRY;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = ost->ost_tgt;

        ptr = ost_req_buf1(req->rq_req.ost);
        req->rq_rep.ost->result = obd_get_info(&conn, req->rq_req.ost->buflen1,
                                               ptr, &vallen, &val);

        rc = ost_pack_rep(val, vallen, NULL, 0, &req->rq_rephdr,
                          &req->rq_rep, &req->rq_replen, &req->rq_repbuf);
        if (rc)
                CERROR("cannot pack reply\n");

        RETURN(rc);
}

static int ost_brw_read(struct ost_obd *obddev, struct ptlrpc_request *req)
{
        struct ptlrpc_bulk_desc **bulk_vec = NULL;
        struct ptlrpc_bulk_desc *bulk = NULL;
        struct obd_conn conn;
        int rc;
        int i, j;
        int objcount, niocount;
        char *tmp1, *tmp2, *end2;
        int cmd;
        struct niobuf *nb, *dst, *res = NULL;
        struct obd_ioobj *ioo;
        struct ost_req *r = req->rq_req.ost;

        ENTRY;

        tmp1 = ost_req_buf1(r);
        tmp2 = ost_req_buf2(r);
        end2 = tmp2 + req->rq_req.ost->buflen2;
        objcount = r->buflen1 / sizeof(*ioo);
        niocount = r->buflen2 / sizeof(*nb);
        cmd = r->cmd;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = req->rq_obd->u.ost.ost_tgt;

        for (i = 0; i < objcount; i++) {
                ost_unpack_ioo((void *)&tmp1, &ioo);
                if (tmp2 + ioo->ioo_bufcnt > end2) {
                        LBUG();
                        rc = -EFAULT;
                        break;
                }
                for (j = 0; j < ioo->ioo_bufcnt; j++)
                        ost_unpack_niobuf((void *)&tmp2, &nb);
        }

        rc = ost_pack_rep(NULL, 0, NULL, 0,
                          &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }
        OBD_ALLOC(res, sizeof(*res) * niocount);
        if (res == NULL)
                RETURN(-ENOMEM);

        /* The unpackers move tmp1 and tmp2, so reset them before using */
        tmp1 = ost_req_buf1(r);
        tmp2 = ost_req_buf2(r);
        req->rq_rep.ost->result = obd_preprw
                (cmd, &conn, objcount, (struct obd_ioobj *)tmp1,
                 niocount, (struct niobuf *)tmp2, res);

        if (req->rq_rep.ost->result)
                GOTO(out, 0);

        for (i = 0; i < niocount; i++) {
                bulk = ptlrpc_prep_bulk(&req->rq_peer);
                if (bulk == NULL) {
                        CERROR("cannot alloc bulk desc\n");
                        rc = -ENOMEM;
                        GOTO(out, rc);
                }

                dst = &((struct niobuf *)tmp2)[i];
                bulk->b_xid = dst->xid;
                bulk->b_buf = (void *)(unsigned long)res[i].addr;
                bulk->b_buflen = PAGE_SIZE;
                rc = ptlrpc_send_bulk(bulk, OST_BULK_PORTAL);
                if (rc)
                        GOTO(out, rc);
                wait_event_interruptible(bulk->b_waitq,
                                         ptlrpc_check_bulk_sent(bulk));

                if (bulk->b_flags == PTL_RPC_INTR)
                        GOTO(out, 0);

                OBD_FREE(bulk, sizeof(*bulk));
                bulk = NULL;
        }

#if 0
        /* Local delivery */
        dst = &((struct niobuf *)tmp2)[i];
        memcpy((void *)(unsigned long)dst->addr,
               (void *)(unsigned long)src->addr, PAGE_SIZE);
#endif
        barrier();

        /* The unpackers move tmp1 and tmp2, so reset them before using */
        tmp1 = ost_req_buf1(r);
        tmp2 = ost_req_buf2(r);
        req->rq_rep.ost->result = obd_commitrw
                (cmd, &conn, objcount, (struct obd_ioobj *)tmp1,
                 niocount, res);

        EXIT;
 out:
        if (res != NULL)
                OBD_FREE(res, sizeof(*res) * niocount);
        if (bulk != NULL)
                OBD_FREE(bulk, sizeof(*bulk));
        if (bulk_vec != NULL) {
                for (i = 0; i < niocount; i++) {
                        if (bulk_vec[i] != NULL)
                                OBD_FREE(bulk_vec[i], sizeof(*bulk));
                }
                OBD_FREE(bulk_vec, niocount * sizeof(*bulk_vec));
        }

        return 0;
}

static int ost_commit_page(struct obd_conn *conn, struct page *page)
{
        struct obd_ioobj obj;
        struct niobuf buf;
        int rc;
        ENTRY;

        memset(&buf, 0, sizeof(buf));
        memset(&obj, 0, sizeof(obj));

        buf.page = page;
        obj.ioo_bufcnt = 1;

        rc = obd_commitrw(OBD_BRW_WRITE, conn, 1, &obj, 1, &buf);
        RETURN(rc);
}

static int ost_brw_write_cb(struct ptlrpc_bulk_desc *bulk, void *data)
{
        int rc;

        ENTRY;

        rc = ost_commit_page(&bulk->b_conn, bulk->b_page);
        if (rc)
                CERROR("ost_commit_page failed: %d\n", rc);

        RETURN(rc);
}

static int ost_brw_write(struct ost_obd *obddev, struct ptlrpc_request *req)
{
        struct obd_conn conn;
        int rc;
        int i, j;
        int objcount, niocount;
        char *tmp1, *tmp2, *end2;
        char *res;
        int cmd;
        struct niobuf *nb, *dst;
        struct obd_ioobj *ioo;
        struct ost_req *r = req->rq_req.ost;

        ENTRY;

        tmp1 = ost_req_buf1(r);
        tmp2 = ost_req_buf2(r);
        end2 = tmp2 + req->rq_req.ost->buflen2;
        objcount = r->buflen1 / sizeof(*ioo);
        niocount = r->buflen2 / sizeof(*nb);
        cmd = r->cmd;

        conn.oc_id = req->rq_req.ost->connid;
        conn.oc_dev = req->rq_obd->u.ost.ost_tgt;

        for (i = 0; i < objcount; i++) {
                ost_unpack_ioo((void *)&tmp1, &ioo);
                if (tmp2 + ioo->ioo_bufcnt > end2) {
                        rc = -EFAULT;
                        break;
                }
                for (j = 0; j < ioo->ioo_bufcnt; j++)
                        ost_unpack_niobuf((void *)&tmp2, &nb);
        }

        rc = ost_pack_rep(NULL, 0, NULL, niocount * sizeof(*nb),
                          &req->rq_rephdr, &req->rq_rep,
                          &req->rq_replen, &req->rq_repbuf);
        if (rc) {
                CERROR("cannot pack reply\n");
                RETURN(rc);
        }
        res = ost_rep_buf2(req->rq_rep.ost);

        /* The unpackers move tmp1 and tmp2, so reset them before using */
        tmp1 = ost_req_buf1(r);
        tmp2 = ost_req_buf2(r);
        req->rq_rep.ost->result = obd_preprw
                (cmd, &conn, objcount, (struct obd_ioobj *)tmp1,
                 niocount, (struct niobuf *)tmp2, (struct niobuf *)res);

        if (req->rq_rep.ost->result)
                GOTO(out, 0);

        for (i = 0; i < niocount; i++) {
                struct ptlrpc_bulk_desc *bulk;
                struct ptlrpc_service *srv = req->rq_obd->u.ost.ost_service;

                bulk = ptlrpc_prep_bulk(&req->rq_peer);
                if (bulk == NULL) {
                        CERROR("cannot alloc bulk desc\n");
                        rc = -ENOMEM;
                        GOTO(out, rc);
                }

                spin_lock(&srv->srv_lock);
                bulk->b_xid = srv->srv_xid++;
                spin_unlock(&srv->srv_lock);

                dst = &((struct niobuf *)res)[i];
                dst->xid = HTON__u32(bulk->b_xid);

                bulk->b_buf = (void *)(unsigned long)dst->addr;
                bulk->b_cb = ost_brw_write_cb;
                bulk->b_page = dst->page;
                memcpy(&(bulk->b_conn), &conn, sizeof(conn));
                bulk->b_buflen = PAGE_SIZE;
                bulk->b_portal = OSC_BULK_PORTAL;
                rc = ptlrpc_register_bulk(bulk);
                if (rc)
                        GOTO(out, rc);

#if 0
                /* Local delivery */
                src = &((struct niobuf *)tmp2)[i];
                memcpy((void *)(unsigned long)dst->addr,
                       (void *)(unsigned long)src->addr, src->len);
#endif
        }
        barrier();

        EXIT;
 out:
        /* FIXME: should we return 'rc' here? */
        return 0;
}

static int ost_brw(struct ost_obd *obddev, struct ptlrpc_request *req)
{
        struct ost_req *r = req->rq_req.ost;
        int cmd = r->cmd;

        if (cmd == OBD_BRW_READ)
                return ost_brw_read(obddev, req);
        else
                return ost_brw_write(obddev, req);
}

static int ost_handle(struct obd_device *obddev, struct ptlrpc_service *svc,
                      struct ptlrpc_request *req)
{
        int rc;
        struct ost_obd *ost = &obddev->u.ost;
        struct ptlreq_hdr *hdr;

        ENTRY;

        hdr = (struct ptlreq_hdr *)req->rq_reqbuf;
        if (NTOH__u32(hdr->type) != PTL_RPC_REQUEST) {
                CERROR("lustre_ost: wrong packet type sent %d\n",
                       NTOH__u32(hdr->type));
                LBUG();
                rc = -EINVAL;
                GOTO(out, rc);
        }

        rc = ost_unpack_req(req->rq_reqbuf, req->rq_reqlen,
                            &req->rq_reqhdr, &req->rq_req);
        if (rc) {
                CERROR("lustre_ost: Invalid request\n");
                GOTO(out, rc);
        }

        switch (req->rq_reqhdr->opc) {

        case OST_CONNECT:
                CDEBUG(D_INODE, "connect\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_CONNECT_NET, 0);
                rc = ost_connect(ost, req);
                break;
        case OST_DISCONNECT:
                CDEBUG(D_INODE, "disconnect\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_DISCONNECT_NET, 0);
                rc = ost_disconnect(ost, req);
                break;
        case OST_GET_INFO:
                CDEBUG(D_INODE, "get_info\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_GET_INFO_NET, 0);
                rc = ost_get_info(ost, req);
                break;
        case OST_CREATE:
                CDEBUG(D_INODE, "create\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_CREATE_NET, 0);
                rc = ost_create(ost, req);
                break;
        case OST_DESTROY:
                CDEBUG(D_INODE, "destroy\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_DESTROY_NET, 0);
                rc = ost_destroy(ost, req);
                break;
        case OST_GETATTR:
                CDEBUG(D_INODE, "getattr\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_GETATTR_NET, 0);
                rc = ost_getattr(ost, req);
                break;
        case OST_SETATTR:
                CDEBUG(D_INODE, "setattr\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_SETATTR_NET, 0);
                rc = ost_setattr(ost, req);
                break;
        case OST_OPEN:
                CDEBUG(D_INODE, "setattr\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_OPEN_NET, 0);
                rc = ost_open(ost, req);
                break;
        case OST_CLOSE:
                CDEBUG(D_INODE, "setattr\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_CLOSE_NET, 0);
                rc = ost_close(ost, req);
                break;
        case OST_BRW:
                CDEBUG(D_INODE, "brw\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_BRW_NET, 0);
                rc = ost_brw(ost, req);
                break;
        case OST_PUNCH:
                CDEBUG(D_INODE, "punch\n");
                OBD_FAIL_RETURN(OBD_FAIL_OST_PUNCH_NET, 0);
                rc = ost_punch(ost, req);
                break;
        default:
                req->rq_status = -ENOTSUPP;
                rc = ptlrpc_error(obddev, svc, req);
                RETURN(rc);
        }

        EXIT;
out:
        req->rq_status = rc;
        if (rc) {
                CERROR("ost: processing error %d\n", rc);
                ptlrpc_error(obddev, svc, req);
        } else {
                CDEBUG(D_INODE, "sending reply\n");
                ptlrpc_reply(obddev, svc, req);
        }

        return 0;
}


/* mount the file system (secretly) */
static int ost_setup(struct obd_device *obddev, obd_count len,
                        void *buf)

{
        struct obd_ioctl_data* data = buf;
        struct ost_obd *ost = &obddev->u.ost;
        struct obd_device *tgt;
        int err;
        ENTRY;

        if (data->ioc_dev  < 0 || data->ioc_dev > MAX_OBD_DEVICES)
                RETURN(-ENODEV);

        MOD_INC_USE_COUNT;
        tgt = &obd_dev[data->ioc_dev];
        ost->ost_tgt = tgt;
        if ( ! (tgt->obd_flags & OBD_ATTACHED) ||
             ! (tgt->obd_flags & OBD_SET_UP) ){
                CERROR("device not attached or not set up (%d)\n",
                       data->ioc_dev);
                GOTO(error_dec, err = -EINVAL);
        }

        ost->ost_conn.oc_dev = tgt;
        err = obd_connect(&ost->ost_conn);
        if (err) {
                CERROR("fail to connect to device %d\n", data->ioc_dev);
                GOTO(error_dec, err = -EINVAL);
        }

        ost->ost_service = ptlrpc_init_svc(128 * 1024,
                                           OST_REQUEST_PORTAL, OSC_REPLY_PORTAL,
                                           "self", ost_handle);
        if (!ost->ost_service) {
                CERROR("failed to start service\n");
                GOTO(error_disc, err = -EINVAL);
        }

        err = ptlrpc_start_thread(obddev, ost->ost_service, "lustre_ost");
        if (err) {
                GOTO(error_disc, err = -EINVAL);
        }

        RETURN(0);

error_disc:
        obd_disconnect(&ost->ost_conn);
error_dec:
        MOD_DEC_USE_COUNT;
        RETURN(err);
}

static int ost_cleanup(struct obd_device * obddev)
{
        struct ost_obd *ost = &obddev->u.ost;
        int err;

        ENTRY;

        if ( !list_empty(&obddev->obd_gen_clients) ) {
                CERROR("still has clients!\n");
                RETURN(-EBUSY);
        }

        ptlrpc_stop_thread(ost->ost_service);
        rpc_unregister_service(ost->ost_service);

        if (!list_empty(&ost->ost_service->srv_reqs)) {
                // XXX reply with errors and clean up
                CERROR("Request list not empty!\n");
        }
        OBD_FREE(ost->ost_service, sizeof(*ost->ost_service));

        err = obd_disconnect(&ost->ost_conn);
        if (err) {
                CERROR("lustre ost: fail to disconnect device\n");
                RETURN(-EINVAL);
        }

        MOD_DEC_USE_COUNT;
        RETURN(0);
}

/* use obd ops to offer management infrastructure */
static struct obd_ops ost_obd_ops = {
        o_setup:       ost_setup,
        o_cleanup:     ost_cleanup,
};

static int __init ost_init(void)
{
        obd_register_type(&ost_obd_ops, LUSTRE_OST_NAME);
        return 0;
}

static void __exit ost_exit(void)
{
        obd_unregister_type(LUSTRE_OST_NAME);
}

MODULE_AUTHOR("Peter J. Braam <braam@clusterfs.com>");
MODULE_DESCRIPTION("Lustre Object Storage Target (OST) v0.01");
MODULE_LICENSE("GPL");

module_init(ost_init);
module_exit(ost_exit);
